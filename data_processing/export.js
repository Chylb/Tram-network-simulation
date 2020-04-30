module.exports = {
    shortenIds: function (physicalNetwork, logicalNetwork) {

        for (let route of logicalNetwork.routes) {
            route.stops = route.stops.map(id => physicalNetwork.nodes.get(id));
        }

        const idGen = idMaker();

        for (let [id, node] of physicalNetwork.nodes) {
            node.id = idGen.next().value;
        }

        for (let track of physicalNetwork.tracks) {
            track.id = idGen.next().value;
        }

        for (let route of logicalNetwork.routes) {
            route.stops = route.stops.map(node => node.id);
        }

        function* idMaker() {
            let id = 0;
            while (true)
                yield id++;
        }
    },

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
                if (node.hasOwnProperty("tags")) {
                    export_node.stopName = node.tags.name;
                }
                if (node.hasOwnProperty("trafficLight"))
                    export_node.trafficLight = node.trafficLight;
                export_data.nodes.push(export_node);
            }
            else
                for (let track of physicalNetwork.tracks)
                    if (track.nodes[0].id == node.id || track.nodes[track.nodes.length - 1].id == node.id) {
                        export_data.nodes.push({
                            x: node.x,
                            y: node.y,
                            id: node.id
                        });
                        break;
                    }
        }

        for (let track of physicalNetwork.tracks) {
            const head = track.nodes[track.nodes.length - 1];
            const tail = track.nodes[0];

            const edge = {
                id: track.id,
                head: head.id,
                tail: tail.id,
                length: Math.sqrt((head.x - tail.x) ** 2 + (head.y - tail.y) ** 2),
                maxspeed: track.tags.maxspeed
            };

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
    makeNetworkVisualizationModel: function (physicalNetwork, logicalNetwork){}
};