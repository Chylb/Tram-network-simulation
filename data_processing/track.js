const findPath = require("./graph.js").findPath;

module.exports = {
    removeTrackCrossings: function (data) { //splits nodes with 4 adjacent nodes to 2 nodes with 2 adjacent nodes
        for (let ji = data.joints.length - 1; ji >= 0; ji--) {
            const j = data.joints[ji];

            if (j.adjacentNodes.length != 4)
                continue;

            if (j.accessibleNodes.length == 4)
                continue;

            const vectors = [];
            for (let i = 0; i < 4; i++) {
                const vec = {
                    x: j.adjacentNodes[i].x - j.x,
                    y: j.adjacentNodes[i].y - j.y
                };
                vec.mag = Math.sqrt(vec.x ** 2 + vec.y ** 2);
                vectors.push(vec);
            }

            const paths = [];
            for (let i = 0; i < 3; i++) {
                for (let k = i + 1; k < 4; k++) {
                    const path = {
                        ix1: i,
                        ix2: k,
                        accessible: 0,
                        cos: (vectors[i].x * vectors[k].x + vectors[i].y * vectors[k].y) / (vectors[i].mag * vectors[k].mag)
                    };
                    if (j.accessibleNodes.includes(j.adjacentNodes[i]) && !j.accessibleNodes.includes(j.adjacentNodes[k])) {
                        path.accessible = i;
                        paths.push(path);
                    }
                    else if (!j.accessibleNodes.includes(j.adjacentNodes[i]) && j.accessibleNodes.includes(j.adjacentNodes[k])) {
                        path.accessible = k;
                        paths.push(path);
                    }
                }
            }

            let min = 1;
            let minIx;
            for (let i = 0; i < paths.length; i++) {
                if (paths[i].cos < min) {
                    min = paths[i].cos;
                    minIx = i;
                }
            }
            const path1 = paths[minIx];

            paths.splice(minIx, 1);

            min = 1;
            for (let i = 0; i < paths.length; i++) {
                if (paths[i].cos < min) {
                    min = paths[i].cos;
                    minIx = i;
                }
            }
            const path2 = paths[minIx];

            const j1 = {
                id: findAvaibleNodeID(data),
                x: j.x,
                y: j.y,
                adjacentNodes: [j.adjacentNodes[path1.ix1], j.adjacentNodes[path1.ix2]],
                accessibleNodes: [j.adjacentNodes[path1.accessible]]
            }
            data.nodes.set(j1.id, j1);

            const j2 = {
                id: findAvaibleNodeID(data),
                x: j.x,
                y: j.y,
                adjacentNodes: [j.adjacentNodes[path2.ix1], j.adjacentNodes[path2.ix2]],
                accessibleNodes: [j.adjacentNodes[path2.accessible]]
            }
            data.nodes.set(j2.id, j2);                  

            for (let adj of j.adjacentNodes) {
                for (let k = 0; k < adj.accessibleNodes.length; k++) {
                    const adjAdj = adj.accessibleNodes[k];
                    if (j1.adjacentNodes.includes(adj)) {
                        if (adjAdj == j)
                            adj.accessibleNodes[k] = j1;
                    }
                    else {
                        if (adjAdj == j)
                            adj.accessibleNodes[k] = j2;
                    }
                }

                for (let k = 0; k < adj.adjacentNodes.length; k++) {
                    const adjAdj = adj.adjacentNodes[k];
                    if (j1.adjacentNodes.includes(adj)) {
                        if (adjAdj == j)
                            adj.adjacentNodes[k] = j1;
                    }
                    else {
                        if (adjAdj == j)
                            adj.adjacentNodes[k] = j2;
                    }
                }
            }
            data.nodes.delete(j.id);
            data.joints.splice(ji, 1);
        }
    },

    generateOppositeEdgesToBidirectionalTracks: function (data) {
        const bidirectionalTracks = [];
        bidirectionalTracks.push({
            nodes: [1770978486, ...findPath(data.nodes.get(1770978486), [213578407])[1].reverse()],
            in1: 4556178680,
            out1: 1770978488,
            in2: 1770978489,
            out2: 4556178677
        });

        bidirectionalTracks.push({
            nodes: [213578409, ...findPath(data.nodes.get(213578409), [1770978496])[1].reverse()],
            in1: 4556178684,
            out1: 1770978500,
            in2: 1770978502,
            out2: 4556178678
        });

        for (let biTrack of bidirectionalTracks) {
            biTrack.nodes = biTrack.nodes.map(id => data.nodes.get(id));
            biTrack.in1 = data.nodes.get(biTrack.in1);
            biTrack.out1 = data.nodes.get(biTrack.out1);
            biTrack.in2 = data.nodes.get(biTrack.in2);
            biTrack.out2 = data.nodes.get(biTrack.out2);

            biTrack.in1.accessibleNodes = [biTrack.nodes[0]];
            for (let i = 0; i < biTrack.nodes.length - 1; i++) {
                const node = biTrack.nodes[i];
                const nextNode = biTrack.nodes[i + 1];
                node.accessibleNodes = [nextNode];
            }
            biTrack.nodes[biTrack.nodes.length - 1].accessibleNodes = [biTrack.out1];

            let oppositeEdge = [];
            for (let i = 0; i < biTrack.nodes.length; i++) {
                const node = biTrack.nodes[i];
                const oppositeNode = {
                    id: findAvaibleNodeID(data),
                    x: node.x,
                    y: node.y,
                    accessibleNodes: [],
                    adjacentNodes: node.adjacentNodes
                }
                data.nodes.set(oppositeNode.id, oppositeNode);
                oppositeEdge.push(oppositeNode);
            }
            oppositeEdge = oppositeEdge.reverse();

            biTrack.in2.accessibleNodes = [oppositeEdge[0]];
            for (let i = 0; i < biTrack.nodes.length - 1; i++) {
                const node = oppositeEdge[i];
                const nextNode = oppositeEdge[i + 1];
                node.accessibleNodes = [nextNode];
            }
            oppositeEdge[oppositeEdge.length - 1].accessibleNodes = [biTrack.out2];
        }
    },

    regenerateTracks: function (data) { //generates tracks based on graph 
        const specialNodes = [];

        //specialNodes.push(data.nodes.get(2384741817));

        data.nodes.get(2384741817).special = true;

        data.nodes.get(2163355821).special = true;
        data.nodes.get(3161355030).special = true;

        for (let [id, node] of data.nodes) {
            if(!node.hasOwnProperty("special"))
                node.special = false;

            if (node.adjacentNodes.length != 2 ||
                node.accessibleNodes.length != 1 ||
                node.hasOwnProperty("tags") ||
                node.hasOwnProperty("trafficLight") ||
                node.special
            ) {
                node.special = true;
                specialNodes.push(node);
            }
        }

        const newTracks = [];
        let id = 0;

        for (firstNode of specialNodes) {
            for (let secondNode of firstNode.accessibleNodes) {
                const track = {
                    id: id,
                    nodes: [firstNode, secondNode],
                    tags: {
                        maxspeed: 50
                    }
                };

                let node = secondNode;

                while (!node.special) {
                    node = node.accessibleNodes[0];
                    track.nodes.push(node);
                }

                let length = 0;
                let prevNode = track.nodes[0];
                for(let i = 1; i < track.nodes.length; i++) {
                    const node = track.nodes[i];
                    length += Math.sqrt( (node.x - prevNode.x)**2 + (node.y - prevNode.y)**2);
                    prevNode = node;
                }
                track.length = length;

                newTracks.push(track);
                ++id;
            }
        }

        data.tracks = newTracks;
    }
};

function findAvaibleNodeID(data) {
    let id = 0;
    while (data.nodes.get(id) != undefined)
        id++;

    return id;
}