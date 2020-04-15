module.exports = {
    makeNetworkModel: function (physicalNetwork, logicalNetwork) { //transforms data to simulation-friendly tram network model
        const export_data = {
            nodes: [],
            edges: [],
            junctions: [],
            routes: [],
            trips: []
        };

        for (let [id, node] of physicalNetwork.nodes) {
            if (node.hasOwnProperty("tags") || node.hasOwnProperty("trafficLight")) {
                const export_node = {
                    x: node.x,
                    y: node.y,
                    id: node.id
                };
                if (node.hasOwnProperty("tags"))
                    export_node.stopName = node.tags.name;
                if (node.hasOwnProperty("trafficLight"))
                    export_node.trafficLight = node.trafficLight;
                export_data.nodes.push(export_node);
            }
            else
                for (let track of physicalNetwork.tracks)
                    if (track.nodes[0] == node.id || track.nodes[track.nodes.length - 1] == node.id) {
                        export_data.nodes.push({
                            x: node.x,
                            y: node.y,
                            id: node.id
                        });
                        break;
                    }
        }

        for (let track of physicalNetwork.tracks) {
            const head = physicalNetwork.nodes.get(track.nodes[track.nodes.length - 1]);
            const tail = physicalNetwork.nodes.get(track.nodes[0]);
            const edge = {
                head: head.id,
                tail: tail.id,
                length: Math.sqrt((head.x - tail.x) ** 2 + (head.y - tail.y) ** 2),
                maxspeed: track.tags.maxspeed
            };

            if (track.tags.oneway == "no") {
                const edge2 = {
                    head: tail.id,
                    tail: head.id,
                    length: Math.sqrt((head.x - tail.x) ** 2 + (head.y - tail.y) ** 2),
                    maxspeed: track.tags.maxspeed
                };
                export_data.edges.push(edge2);
            }
            export_data.edges.push(edge);
        }

        for (let junction of physicalNetwork.junctions) {
            const export_junction = {
                trafficLights: junction.trafficLights.map(x => x.id)
            };
            export_data.junctions.push(export_junction);
        }

        for (let route of logicalNetwork.routes) {
            const export_route = {
                id: route.id,
                name: route.scheduleRoute.name,
                stops: route.stops
            };
            export_data.routes.push(export_route);
        }

        for (let trip of logicalNetwork.trips) {
            const export_trip = {
                route: trip.route,
                times: trip.times
            };
            export_data.trips.push(export_trip);
        }

        return export_data;
    },

    makeNetworkVisualizationModel(physicalNetwork, logicalNetwork) {}
};