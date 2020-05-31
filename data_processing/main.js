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

  const physicalNetwork = processPhysicalNetwork(osm_tram_data);
  const logicalNetwork = processLogicalNetwork(physicalNetwork, schedule);

  Export.shortenIds(physicalNetwork, logicalNetwork);
  const network_model = JSON.stringify(Export.makeNetworkModel(physicalNetwork, logicalNetwork));
  const network_visualization_model = JSON.stringify(Export.makeNetworkVisualizationModel(physicalNetwork, logicalNetwork));

  fs.writeFileSync("data/network_model.json", network_model);
  fs.writeFileSync("data/network_visualization_model.json", network_visualization_model);
}

main();

function processPhysicalNetwork(osm_tram_data) { //creates physical network
  const pn = { //physical network
    nodes: new Map(),
    stops: [],
    stopsIds: new Map(), //stop name => stop ids
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
      pn.nodes.set(e.id, e);

      if (e.hasOwnProperty('tags'))
        if (e.tags.railway == 'tram_stop')
          stopsTmp.push(e);
    }
    else if (e.type = 'way')
      if (e.hasOwnProperty('nodes'))
        pn.tracks.push(e);
  }

  Fix.fixMissingStops(pn, stopsTmp);

  for (let s of stopsTmp) {
    const node = pn.nodes.get(s.id);
    node.tags = s.tags;
    pn.stops.push(node);
  }

  Fix.fixStopsNames(pn);
  Fix.fixWaysDirections(pn);
  Fix.fixMissingMaxspeed(pn);
  Fix.removeBannedNodes(pn);

  for (let s of pn.stops) {
    const currIds = pn.stopsIds.get(s.tags.name);

    if (currIds == undefined)
      pn.stopsIds.set(s.tags.name, [s.id]);
    else {
      currIds.push(s.id);
      pn.stopsIds.set(s.tags.name, currIds);
    }
  }

  for (let track of pn.tracks) {
    track.nodes = track.nodes.map(id => pn.nodes.get(id));
  }

  Graph.findAdjacentNodes(pn);
  Graph.findSuccessorNodes(pn);
  Graph.manualSuccessorNodesAdjustments(pn);

  Fix.removeFloatingIslands(pn);

  for (let [id, n] of pn.nodes)
    if (n.adjacentNodes.length > 2) {
      pn.joints.push(n);
      n.junction = undefined;
    }

  Track.removeTrackCrossings(pn);
  Track.generateOppositeEdgesToBidirectionalTracks(pn);

  Junction.findJunctions(pn);
  Junction.generateTrafficLights(pn);
  Junction.manualJunctionAdjustments(pn);

  Track.regenerateTracks(pn);

  return pn;
}

function processLogicalNetwork(physicalNetwork, raw_schedule) { //creates logical network
  const schedule = processRawSchedule(raw_schedule);

  const ln = { //logical network
    routes: [],
    trips: []
  };

  Fix.removeFakeRouteStops(schedule);

  Schedule.createRoutes(physicalNetwork, schedule, ln);
  Schedule.createTrips(ln);

  return ln;
}

function processRawSchedule(rawSchedule) { //some trimming of raw data etc.
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






