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
                type: "node",
                id: 0,
                x: j.x,
                y: j.y,
                adjacentNodes: [j.adjacentNodes[path1.ix1], j.adjacentNodes[path1.ix2]],
                accessibleNodes: [j.adjacentNodes[path1.accessible]]
            }

            const j2 = {
                type: "node",
                id: 0,
                x: j.x,
                y: j.y,
                adjacentNodes: [j.adjacentNodes[path2.ix1], j.adjacentNodes[path2.ix2]],
                accessibleNodes: [j.adjacentNodes[path2.accessible]]
            }

            j.j1 = j1;
            j.j2 = j2;

            let id = 0;
            while (data.nodes.get(id) != undefined)
                id++;
            j1.id = id;
            data.nodes.set(id, j1);

            while (data.nodes.get(id) != undefined)
                id++;
            j2.id = id;
            data.nodes.set(id, j2);

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
            //data.nodes.delete(j.id);
            data.joints.splice(ji, 1);
        }
    },

    splitTracksBySpecialNodes: function (data) { //no track should have special nodes in the middle part. Special nodes i.e. track switches, traffic lights, stops.
        const switchesIds = data.joints.map(x => x.id);
        const stopsIds = data.stops.map(x => x.id);

        const trafficLightsIds = [];
        for (let junction of data.junctions) {
            const junctionTrafficLightsIds = junction.trafficLights.map(x => x.id);
            trafficLightsIds = [...trafficLightsIds, ...junctionTrafficLightsIds];
        }

        const specialNodesIds = [...switchesIds, ...stopsIds, ...trafficLightsIds];

        for (let j = data.tracks.length - 1; j >= 0; j--) {
            const track = data.tracks[j];
            const specialNodesIx = [];
            for (let i = 1; i < track.nodes.length - 1; i++)
                if (specialNodesIds.includes(track.nodes[i]))
                    specialNodesIx.push(i);

            if (specialNodesIx.length == 0) continue;

            const firstSection = {
                nodes: track.nodes.slice(0, specialNodesIx[0] + 1),
                tags: track.tags
            };

            const lastSection = {
                nodes: track.nodes.slice(specialNodesIx[specialNodesIx.length - 1], track.nodes.length),
                tags: track.tags
            };

            data.tracks.push(firstSection);
            data.tracks.push(lastSection);

            for (let i = 1; i < specialNodesIx.length; i++) {
                const section = {
                    nodes: track.nodes.slice(specialNodesIx[i - 1], specialNodesIx[i] + 1),
                    tags: track.tags
                };
                data.tracks.push(section);
            }

            data.tracks.splice(j, 1);
        }
    }
};