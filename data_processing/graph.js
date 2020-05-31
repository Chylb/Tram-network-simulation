module.exports = {
  findAdjacentNodes: function (pn) { //finds adjacent nodes for each node 
    for (let track of pn.tracks) {

      track.nodes[0].adjacentNodes.push(track.nodes[1]);

      for (let i = 1; i < track.nodes.length - 1; i++) {
        const nPrev = track.nodes[i - 1];
        const nCurr = track.nodes[i];
        const nNext = track.nodes[i + 1];

        nCurr.adjacentNodes.push(nPrev);
        nCurr.adjacentNodes.push(nNext);
      }

      track.nodes[track.nodes.length - 1].adjacentNodes.push(track.nodes[track.nodes.length - 2]);
    }
  },

  findSuccessorNodes: function (pn) { //for each node finds direct successor nodes
    for (let track of pn.tracks)
      for (let i = 0; i < track.nodes.length - 1; i++) {
        const node = track.nodes[i];
        const nextNode = track.nodes[i + 1];
        node.accessibleNodes.push(nextNode);
      }
  },

  manualSuccessorNodesAdjustments: function (pn) {
    pn.nodes.get(2756848427).accessibleNodes = [pn.nodes.get(1770194427)];
  },

  findPath: function (source, targets, limit = Number.POSITIVE_INFINITY) { //finds shortest path between source and nearest target
    const distance = new Map();
    const previous = new Map();

    const queue = new PriorityQueue();

    distance.set(source.id, 0);
    for (let adj of source.accessibleNodes) {
      const dis = Math.sqrt((source.x - adj.x) ** 2 + (source.y - adj.y) ** 2);

      distance.set(adj.id, dis);
      previous.set(adj.id, source);
      queue.push(dis, adj);
    }

    let node;
    let dis;

    let foundDis = Number.POSITIVE_INFINITY;
    let foundNode;
    let found = false;

    while (queue.size() > 0) {
      [dis, node] = queue.pop();
      if (dis > limit)
        break;

      if (targets.includes(node.id)) {
        if (!found) {
          found = true;
          foundNode = node;
          foundDis = dis;
        }
        else {
          if (dis < foundDis) {
            foundNode = node;
            foundDis = dis;
          }
        }
      }

      if (dis > foundDis)
        break;

      for (let adj of node.accessibleNodes) {
        const disAdj = distance.get(adj.id);

        if (disAdj == undefined) {
          const newDis = dis + Math.sqrt((node.x - adj.x) ** 2 + (node.y - adj.y) ** 2);

          queue.push(newDis, adj);
          distance.set(adj.id, newDis);
          previous.set(adj.id, node);
        }
      }
    }

    if (!found)
      return [Number.POSITIVE_INFINITY, []];

    const path = [];

    node = foundNode;
    while (node != source) {
      path.push(node.id);
      node = previous.get(node.id);
    }

    return [foundDis, path];
  }
};

class PriorityQueue {
  constructor() {
    this.arr = [];
    this.priority = [];
  }

  push(p, e) {
    if (this.arr.includes(e))
      console.log("WARNING! DUPLICATE ELEMENT");

    let i = 0;
    while (this.priority[i] < p) {
      i++;
    }
    this.arr.splice(i, 0, e);
    this.priority.splice(i, 0, p);
  }

  pop() {
    if (this.arr.length == 0)
      console.log("WARNING! EMPTY QUEUE")
    return [this.priority.shift(), this.arr.shift()];
  }

  size() {
    return this.arr.length;
  }
}