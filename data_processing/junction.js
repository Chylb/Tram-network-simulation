const findPath = require("./graph.js").findPath;

module.exports = {
  findJunctions: function (pn) { //finds junctions and adds junction object to physical network
    for (let joint of pn.joints) {
      if (joint.junction != undefined)
        continue;

      const junction = {
        joints: [joint],
        trafficLights: [],
        exits: []
      }

      joint.junction = junction;
      groupNearbyJoints(joint, junction);

      pn.junctions.push(junction);
    }

    function groupNearbyJoints(node, junction) {
      const searchingDistance = 120;

      for (let j of pn.joints) {
        if (j.junction != undefined)
          continue;

        const dis = Math.sqrt((node.x - j.x) ** 2 + (node.y - j.y) ** 2);
        if (dis < searchingDistance) {
          j.junction = junction;
          junction.joints.push(j);
          groupNearbyJoints(j, junction);
        }
      }
    }
  },

  generateTrafficLights: function (pn) { //for each junction generates traffic lights
    for (let junction of pn.junctions) {

      for (let joint of junction.joints) {
        if (joint.accessibleNodes.length > 1) {
          junction.trafficLights.push(joint);
        }
        else if (joint.adjacentNodes.length > 2) {
          junction.exits.push(joint);
        }
      }

      const trafficLightsIds = junction.trafficLights.map(x => x.id);

      for (let joint of junction.trafficLights) { //removing pointless traffic lights
        let [dis, path] = findPath(joint, trafficLightsIds, 60);
        if (dis < Number.POSITIVE_INFINITY) {
          junction.trafficLights = junction.trafficLights.filter(x => !(x.id == path[0]));
        }
      }

      const exitsIds = junction.exits.map(x => x.id);

      for (let joint of junction.exits) { //removing pointless exits
        let [dis, path] = findPath(joint, exitsIds, 60);
        if (dis < Number.POSITIVE_INFINITY) {
          junction.exits = junction.exits.filter(x => !(x.id == joint.id));
        }
      }

      updateJunction(junction);
    }
  },

  manualJunctionAdjustments: function (pn) {

    const placCentralny = pn.nodes.get(1769087813).junction;
    removeTrafficLight(pn.nodes.get(321437263));
    removeTrafficLight(pn.nodes.get(1763772197));
    removeTrafficLight(pn.nodes.get(321437392));
    removeTrafficLight(pn.nodes.get(213605067));
    assignTrafficLightToJunction(pn.nodes.get(321437328), placCentralny);
    assignTrafficLightToJunction(pn.nodes.get(4232232962), placCentralny);
    removeJunctionExit(pn.nodes.get(321437390));
    removeJunctionExit(pn.nodes.get(321437325));
    removeJunctionExit(pn.nodes.get(1578761764));
    removeJunctionExit(pn.nodes.get(1763772203));
    removeJunctionExit(pn.nodes.get(1763772203));
    assignJunctionExitToJunction(pn.nodes.get(1769087805), placCentralny);
    assignJunctionExitToJunction(pn.nodes.get(1763772195), placCentralny);

    const rondoCzyżyńskie = pn.nodes.get(1763772276).junction;
    removeTrafficLight(pn.nodes.get(1763772276));
    removeTrafficLight(pn.nodes.get(3629965011));
    removeTrafficLight(pn.nodes.get(4037275953));
    removeTrafficLight(pn.nodes.get(3629965006));

    const rondoGrzegórzeckie = pn.nodes.get(3071495540).junction;
    removeTrafficLight(pn.nodes.get(4555585764));

    const dworzecTowarowy = pn.nodes.get(261704060).junction;
    removeTrafficLight(pn.nodes.get(443709635));
    removeJunctionExit(pn.nodes.get(261704020));
    removeJunctionExit(pn.nodes.get(2219344957));
    assignJunctionExitToJunction(pn.nodes.get(2219344973), dworzecTowarowy);

    const tunnelNorthExit = pn.nodes.get(2219344850).junction;
    removeTrafficLight(pn.nodes.get(2219344850));
    assignTrafficLightToJunction(pn.nodes.get(310654627), tunnelNorthExit);
    assignTrafficLightToJunction(pn.nodes.get(2424198701), tunnelNorthExit);

    const rondoMogilskie = pn.nodes.get(1887802516).junction;
    removeTrafficLight(pn.nodes.get(1245993640));
    removeTrafficLight(pn.nodes.get(1887812666));
    assignTrafficLightToJunction(pn.nodes.get(1887802518), rondoMogilskie);
    assignTrafficLightToJunction(pn.nodes.get(2420776737), rondoMogilskie);

    const łagiewniki = pn.nodes.get(2374339737).junction;
    removeJunctionExit(pn.nodes.get(2374339777));
    removeJunctionExit(pn.nodes.get(629106152));

    const nowyBieżanów = pn.nodes.get(287442345).junction;
    removeTrafficLight(pn.nodes.get(287442345));
    removeTrafficLight(pn.nodes.get(1310458659));
    assignTrafficLightToJunction(pn.nodes.get(4555563339), nowyBieżanów);
    assignTrafficLightToJunction(pn.nodes.get(4555563340), nowyBieżanów);

    const bieżanowska = pn.nodes.get(1890503293).junction;
    assignTrafficLightToJunction(pn.nodes.get(1890495563), bieżanowska);
    assignTrafficLightToJunction(pn.nodes.get(5443781407), bieżanowska);
    assignJunctionExitToJunction(pn.nodes.get(289684900), bieżanowska);
    assignJunctionExitToJunction(pn.nodes.get(293619179), bieżanowska);
    assignJunctionExitToJunction(pn.nodes.get(2424582864), bieżanowska);

    const śwGertrudy = {
      trafficLights: [],
      exits: []
    };
    pn.junctions.push(śwGertrudy);
    assignTrafficLightToJunction(pn.nodes.get(2418766246), śwGertrudy);
    assignTrafficLightToJunction(pn.nodes.get(2418766274), śwGertrudy);
    assignTrafficLightToJunction(pn.nodes.get(2418766260), śwGertrudy);
    assignJunctionExitToJunction(pn.nodes.get(2418766245), śwGertrudy);
    assignJunctionExitToJunction(pn.nodes.get(2418766272), śwGertrudy);
    assignJunctionExitToJunction(pn.nodes.get(2418766269), śwGertrudy);

    const wiadukty = pn.nodes.get(1770978420).junction;
    assignJunctionExitToJunction(pn.nodes.get(213578402), wiadukty);
    removeJunctionExit(pn.nodes.get(1770978496));
    removeJunctionExit(pn.nodes.get(213578407));

    const kocmyrzowskaBidirectionalTracks = {
      trafficLights: [],
      exits: []
    };
    pn.junctions.push(kocmyrzowskaBidirectionalTracks);
    assignTrafficLightToJunction(pn.nodes.get(4556178680), kocmyrzowskaBidirectionalTracks);
    assignTrafficLightToJunction(pn.nodes.get(1770978502), kocmyrzowskaBidirectionalTracks);
    assignJunctionExitToJunction(pn.nodes.get(4556178677), kocmyrzowskaBidirectionalTracks);
    assignJunctionExitToJunction(pn.nodes.get(1770978500), kocmyrzowskaBidirectionalTracks);

    removeTrafficLight(pn.nodes.get(1764579369));

    for (let junction of pn.junctions) {
      updateJunction(junction)
    }
  }
};

function removeTrafficLight(trafficLight) {
  if (trafficLight.hasOwnProperty("junction")) {
    const previousJunction = trafficLight.junction;
    previousJunction.trafficLights = previousJunction.trafficLights.filter(x => !(x == trafficLight));
    delete trafficLight.junction;
    delete trafficLight.trafficLight;
  }
}

function removeJunctionExit(exit) {
  if (exit.hasOwnProperty("junction")) {
    const previousJunction = exit.junction;
    previousJunction.exits = previousJunction.exits.filter(x => !(x == exit));
    delete exit.junction;
    delete exit.exit;
  }
}

function assignTrafficLightToJunction(trafficLight, junction) {
  removeTrafficLight(trafficLight);
  trafficLight.junction = junction;
  junction.trafficLights.push(trafficLight);
}

function assignJunctionExitToJunction(exit, junction) {
  removeJunctionExit(exit);
  exit.junction = junction;
  junction.exits.push(exit);
}

function updateJunction(junction) {
  for (let tl of junction.trafficLights) {
    tl.trafficLight = true;
  }
  for (let ex of junction.exits) {
    ex.exit = true;
  }
}