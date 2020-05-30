const fs = require('fs');

const Fix = require("./fix.js");
const Graph = require("./graph.js");
const Track = require("./track.js");
const Junction = require("./junction.js");

const Schedule = require("./schedule.js");

const Export = require("./export.js");

function main() {
  const osm_tram_data = JSON.parse(fs.readFileSync("data/osm_tram_data.json"));
  const schedule = JSON.parse(fs.readFileSync("data/schedule.json"));

  const physicalNetworkData = processPhysicalNetwork(osm_tram_data);
  const logicalNetworkData = processLogicalNetwork(physicalNetworkData, schedule);

  Export.shortenIds(physicalNetworkData, logicalNetworkData);
  const network_model = JSON.stringify(Export.makeNetworkModel(physicalNetworkData, logicalNetworkData));
  const network_visualization_model = JSON.stringify(Export.makeNetworkVisualizationModel(physicalNetworkData, logicalNetworkData));

  fs.writeFileSync("data/network_model.json", network_model);
  fs.writeFileSync("data/network_visualization_model.json", network_visualization_model);
}

main();

function processPhysicalNetwork(osm_tram_data) {
  const data = {
    nodes: new Map(),
    stops: [],
    stopsIds: new Map(),
    tracks: [],
    joints: [],
    junctions: []
  };

  const lon0 = 19.937356; // Cracow longitude
  const lat0 = 50.061700; // Cracow latitude
  const R = 6365828; // Earth's radius

  const stopsTmp = [];

  for (let e of osm_tram_data.elements) { //loading osm elements
    if (e.type == 'node') {
      let relLon = (e.lon - lon0) / 180 * Math.PI;
      let relLat = (e.lat - lat0) / 180 * Math.PI;

      e.y = R * Math.tan(relLat);
      e.x = R * Math.cos(e.lat / 180 * Math.PI) * Math.sin(relLon);
      e.adjacentNodes = [];
      e.accessibleNodes = [];
      data.nodes.set(e.id, e);

      if (e.hasOwnProperty('tags'))
        if (e.tags.railway == 'tram_stop')
          stopsTmp.push(e);
    }
    else if (e.type = 'way')
      if (e.hasOwnProperty('nodes'))
        data.tracks.push(e);
  }

  Fix.fixMissingStops(data, stopsTmp);

  for (let s of stopsTmp) {
    const node = data.nodes.get(s.id);
    node.tags = s.tags;
    data.stops.push(node);
  }

  Fix.fixStopsNames(data);
  Fix.fixWaysDirections(data);
  Fix.fixMissingMaxspeed(data);
  Fix.removeBannedNodes(data);

  for (let s of data.stops) {
    const currIds = data.stopsIds.get(s.tags.name);

    if (currIds == undefined)
      data.stopsIds.set(s.tags.name, [s.id]);
    else {
      currIds.push(s.id);
      data.stopsIds.set(s.tags.name, currIds);
    }
  }
  
  for(let track of data.tracks) {
    track.nodes = track.nodes.map(id => data.nodes.get(id));
  }

  Graph.findAdjacentNodes(data);
  Graph.findSuccessorNodes(data);
  Graph.manualSuccessorNodesAdjustments(data);

  Fix.removeFloatingIslands(data);

  for (let [id, n] of data.nodes)
    if (n.adjacentNodes.length > 2) {
      data.joints.push(n);
      n.junction = undefined;
    }

  Track.removeTrackCrossings(data);
  Track.generateOppositeEdgesToBidirectionalTracks(data);

  Junction.findJunctions(data);
  Junction.generateTrafficLights(data);
  Junction.manualJunctionAdjustments(data);

  Track.regenerateTracks(data);

  return data;
}

function processLogicalNetwork(physicalNetwork, raw_schedule) {
  const schedule = processRawSchedule(raw_schedule);

  const logicalData = {
    routes: [],
    trips: []
  };

  Fix.removeFakeRouteStops(schedule);

  Schedule.createRoutes(physicalNetwork, schedule, logicalData);
  Schedule.createTrips(logicalData);

  return logicalData;
}

function processRawSchedule(rawSchedule) {
  const schedule = JSON.parse(JSON.stringify(rawSchedule));

  for (let l = 0; l < schedule.lines.length; l++) {
    schedule.lines[l].number = schedule.lines[l].number.trim();
    schedule.lines[l].direction1.name = schedule.lines[l].direction1.name.trim();
    schedule.lines[l].direction2.name = schedule.lines[l].direction2.name.trim();

    for (let i = 0; i < schedule.lines[l].direction1.stops.length; i++) {
      schedule.lines[l].direction1.stops[i].name = schedule.lines[l].direction1.stops[i].name.trim();
      let cutIx;
      for (let j = 0; j < schedule.lines[l].direction1.stops[i].schedule.length; j++) {
        if (Number.isNaN(Number(schedule.lines[l].direction1.stops[i].schedule[j].Godzina.charAt(0)))) {
          cutIx = j;
          break;
        }
      }
      schedule.lines[l].direction1.stops[i].schedule = schedule.lines[l].direction1.stops[i].schedule.slice(0, cutIx);
    }

    for (let i = 0; i < schedule.lines[l].direction2.stops.length; i++) {
      schedule.lines[l].direction2.stops[i].name = schedule.lines[l].direction2.stops[i].name.trim();
      let cutIx;
      for (let j = 0; j < schedule.lines[l].direction2.stops[i].schedule.length; j++) {
        if (Number.isNaN(Number(schedule.lines[l].direction2.stops[i].schedule[j].Godzina.charAt(0)))) {
          cutIx = j;
          break;
        }
      }
      schedule.lines[l].direction2.stops[i].schedule = schedule.lines[l].direction2.stops[i].schedule.slice(0, cutIx);
    }
  }

  return schedule;
}






