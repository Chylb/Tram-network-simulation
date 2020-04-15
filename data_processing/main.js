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

  const network_model = JSON.stringify(Export.makeNetworkModel(physicalNetworkData, logicalNetworkData));
  //const network_visualization_model = JSON.stringify(Export.processData2(physicalNetworkData, logicalNetworkData));

  fs.writeFileSync("data/network_model.json", network_model);
  //fs.writeFileSync("network_visualization_model.json", network_visualization_model);
}

main();

function processPhysicalNetwork(osm_tram_data) {
  const data = {
    nodes: new Map(),
    stops: [],
    stopsIds: new Map(),
    tracks: [],
    bidirectionalTracks: [],
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

  for (let s of stopsTmp)
    data.stops.push(s);

  Fix.fixStopsNames(data);
  Fix.removeBannedNodes(data);
  Fix.fixWaysDirections(data);
  Fix.fixMissingMaxspeed(data);

  for (let s of data.stops) {
    const currIds = data.stopsIds.get(s.tags.name);

    if (currIds == undefined)
      data.stopsIds.set(s.tags.name, [s.id]);
    else {
      currIds.push(s.id);
      data.stopsIds.set(s.tags.name, currIds);
    }
  }

  for (let track of data.tracks)
    if (track.tags.oneway == "no")
      data.bidirectionalTracks.push(track);

  Graph.findAdjacentNodes(data);
  Graph.findSuccessorNodes(data);

  for (let [id, n] of data.nodes)
    if (n.adjacentNodes.length > 2) {
      data.joints.push(n);
      n.junction = undefined;
      //n.trafficLight = 0;
    }

  Track.removeTrackCrossings(data);
  Track.splitTracksBySpecialNodes(data);

  Junction.findJunctions(data);
  Junction.generateTrafficLights(data);

  return data;
}

function processLogicalNetwork(physicalNetwork, raw_schedule) {
  const schedule = processRawSchedule(raw_schedule);

  const logicalData = {
    routes: [],
    trips: []
  };

  Fix.removeFakeRouteStops(schedule);

  //for (let i = 0; i < schedule.lines.length; i++) {
  for (let i = 0; i < 1; i++) {
    const route1 = Schedule.processRoute(physicalNetwork, schedule.lines[i].direction1, 213578451, 2419732952);
    const route2 = Schedule.processRoute(physicalNetwork, schedule.lines[i].direction2, 2419732952, 2426087293);
    route1.id = 2 * i;
    route2.id = 2 * i + 1;

    logicalData.routes.push(route1);
    logicalData.routes.push(route2);
  }

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






