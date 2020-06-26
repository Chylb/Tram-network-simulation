module.exports = {
    shortenIds: function (physicalNetwork, logicalNetwork) {

        for (let route of logicalNetwork.routes) {
            route.stops = route.stops.map(id => physicalNetwork.nodes.get(id));
        }

        const idGen = idMaker();

        for (let [id, node] of physicalNetwork.nodes) {
            node.id = idGen.next().value;
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
        const networkModel = {
            nodes: [],
            edges: [],
            junctions: [],
            routes: [],
            trips: []
        };

        for (let [id, node] of physicalNetwork.nodes) {
            if (node.special) {
                const export_node = {
                    x: node.x,
                    y: node.y,
                    id: node.id
                };
                if (node.hasOwnProperty("tags"))
                    export_node.stopName = node.tags.name;
                if (node.hasOwnProperty("trafficLight"))
                    export_node.trafficLight = node.trafficLight;
                if (node.hasOwnProperty("exit"))
                    export_node.exit = node.exit;
                networkModel.nodes.push(export_node);
            }
        }

        for (let track of physicalNetwork.tracks) {
            const head = track.nodes[track.nodes.length - 1];
            const tail = track.nodes[0];

            const export_edge = {
                id: track.id,
                head: head.id,
                tail: tail.id,
                length: track.length,
                maxspeed: track.tags.maxspeed
            };

            networkModel.edges.push(export_edge);
        }

        for (let junction of physicalNetwork.junctions) {
            const export_junction = {
                trafficLights: junction.trafficLights.map(x => x.id),
                exits: junction.exits.map(x => x.id)
            };
            networkModel.junctions.push(export_junction);
        }

        for (let route of logicalNetwork.routes) {
            const export_route = {
                id: route.id,
                name: route.scheduleRoute.name,
                stops: route.stops
            };
            networkModel.routes.push(export_route);
        }

        for (let trip of logicalNetwork.trips) {
            const export_trip = {
                route: trip.route,
                times: trip.times
            };
            networkModel.trips.push(export_trip);
        }

        return networkModel;
    },

    makeNetworkVisualizationModel: function (physicalNetwork) {
        const networkVisualizationModel = {
            nodes: [],
            edges: []
        };

        for (let [id, node] of physicalNetwork.nodes) {
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

            networkVisualizationModel.nodes.push(export_node);
        }

        for (let track of physicalNetwork.tracks) {
            const head = track.nodes[track.nodes.length - 1];
            const tail = track.nodes[0];

            const export_edge = {
                id: track.id,
                nodes: track.nodes.map(node => node.id),
                length: track.length,
                maxspeed: track.tags.maxspeed
            };

            networkVisualizationModel.edges.push(export_edge);
        }

        return networkVisualizationModel;
    }
};