const findPath = require("./graph.js").findPath;

module.exports = {
  findJunctions: function (data) {
    for (let joint of data.joints) {
      if (joint.junction != undefined)
        continue;

      const junction = {
        joints: [joint],
        trafficLights: []
      }

      joint.junction = junction;
      groupNearbyJoints(joint, junction);

      data.junctions.push(junction);
    }

    function groupNearbyJoints(node, junction) {
      const searchingDistance = 120;

      for (let j of data.joints) {
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

  generateTrafficLights: function (data) {
    for (let junction of data.junctions) {

      for (let joint of junction.joints) {
        if (joint.accessibleNodes.length > 1) {
          junction.trafficLights.push(joint);
        }
      }
      
      const trafficLightsIds = junction.trafficLights.map(x => x.id);

      for (let joint of junction.trafficLights) { //removing pointless traffic lights
        let [dis, path] = findPath(joint, trafficLightsIds, 60);
        if (dis < Number.POSITIVE_INFINITY) {
          junction.trafficLights = junction.trafficLights.filter(x => !(x.id == path[0]));
        }
      }
      updateJunction(junction);
    }
  },

  manualJunctionAdjustments: function (data) {
    const newJunction = {
      joints: [],
      trafficLights: []
    };
    data.junctions.push(newJunction);

    changeJunction(data.nodes.get(2374339737), newJunction);
    changeJunction(data.nodes.get(290038770), newJunction);
    changeJunction(data.nodes.get(2374339745), newJunction);
    changeJunction(data.nodes.get(2374339754), newJunction);
  }
};

function changeJunction(trafficLight, newJunction) {
  const previousJunction = trafficLight.junction;
  trafficLight.junction.trafficLights = trafficLight.junction.trafficLights.filter(x => !(x == trafficLight));
  newJunction.trafficLights.push(trafficLight);

  updateJunction(previousJunction);
  updateJunction(newJunction);
}

function updateJunction(junction) {
  let trafficLightCount = 0;
  for (let tl of junction.trafficLights) {
    tl.trafficLight = trafficLightCount;
    trafficLightCount++;
  }
  junction.trafficLightCount = trafficLightCount;
}