const findPath = require("./graph.js").findPath;

module.exports = {
    createRoutes: function (physicalNetwork, schedule, logicalNetwork) {
        routeNodeStart = new Map(); //for given tram stop name (routeNode) designates specific tram stop used as a start of a route. Used by default.
        routeStart = new Map(); //for given route designates specific tram stop used as a start of a route. Used in certain circumstances.

        routeNodeStart.set("Borek Fałęcki", 321429867);
        routeNodeStart.set("Bronowice Małe", 2419986540);
        routeNodeStart.set("Bronowice SKA", 2419986540);
        routeNodeStart.set("Cichy Kącik", 2419831874);
        routeNodeStart.set("Cmentarz Rakowicki", 321458378);
        routeNodeStart.set("Czerwone Maki P+R", 2017955951);
        routeNodeStart.set("Dajwór", 3526833498);
        routeNodeStart.set("Dworzec Towarowy", 2420286330);
        routeNodeStart.set("Krowodrza Górka", 257655889);
        routeNodeStart.set("Kurdwanów P+R", 5728702340);
        routeNodeStart.set("Mały Płaszów P+R", 1889519517);
        routeNodeStart.set("Mistrzejowice", 213648443);
        routeNodeStart.set("Nowy Bieżanów P+R", 287586332);
        routeNodeStart.set("Os. Piastów", 213646624);
        routeNodeStart.set("Piasta Kołodzieja", 2427215149);
        routeNodeStart.set("Pleszów", 1764757832);
        routeNodeStart.set("Salwator", 2419732952);
        routeNodeStart.set("TAURON Arena Kraków Wieczysta", 4069652993);
        routeNodeStart.set("Witosa", 289685344);
        routeNodeStart.set("Wzgórza Krzesławickie", 213578451);

        routeStart.set("Kopiec Wandy - Borek Fałęcki", 773635797);
        routeStart.set("Łagiewniki - TAURON Arena Kraków Wieczysta", 629106151);

        let id = 0;

        for (let line of schedule.lines) {
            dir1Start = routeStart.get(line.direction1.name);
            if (dir1Start == undefined)
                dir1Start = routeNodeStart.get(line.direction1.stops[0].name);
            dir1End = line.direction2.stops[0].name;

            dir2Start = routeStart.get(line.direction2.name);
            if (dir2Start == undefined)
                dir2Start = routeNodeStart.get(line.direction2.stops[0].name);
            dir2End = line.direction1.stops[0].name;

            let route1;
            let route2;

            if (dir1Start !== undefined && dir1End !== undefined) {
                route1 = processRoute(physicalNetwork, line.direction1, dir1Start, dir1End);
                route1.id = id;
                id++;
            }
            else
                console.log("No " + line.direction1.name + " endpoints given");

            if (dir2Start !== undefined && dir2End !== undefined) {
                route2 = processRoute(physicalNetwork, line.direction2, dir2Start, dir2End);
                route2.id = id;
                id++;
            }
            else
                console.log("No " + line.direction2.name + " endpoints given");

            if (route1 != undefined)
                logicalNetwork.routes.push(route1);

            if (route2 != undefined)
                logicalNetwork.routes.push(route2);
        }
    },

    createTrips: function (logicalNetwork) {
        for (let route of logicalNetwork.routes) {
            const times = [];

            for (let i = 0; i < route.scheduleRoute.stops.length; i++) {
                const stop = route.scheduleRoute.stops[i];
                times[i] = [];

                for (let j = 0; j < stop.schedule.length; j++) {
                    let h = parseInt(stop.schedule[j].Godzina);
                    if (isNaN(h))
                        continue;

                    if (h < 2)
                        h += 24;

                    let ms;
                    if (stop.schedule[j].hasOwnProperty("Dzień powszedni"))
                        ms = stop.schedule[j]["Dzień powszedni"].split(" ");
                    else
                        ms = [];

                    for (let m of ms) {
                        const mn = parseInt(m);
                        if (isNaN(mn))
                            continue;

                        times[i].push(h * 60 + mn);
                    }
                }
            }

            let tripCount = times[0].length;
            for (let row of times)
                if (row.length > tripCount)
                    tripCount = row.length;
            const stopCount = times.length;

            for (let i = 0; i < tripCount; i++) {
                const trip = {
                    route: route.id,
                    times: [],
                    start: 0,
                    end: 0,
                    duration: 0
                };

                let lastStopTime = -1;

                for (let j = 0; j < stopCount; j++) {
                    let k = 0;

                    while (k < times[j].length) {
                        if (times[j][k] > lastStopTime && (lastStopTime == -1 || times[j][k] - lastStopTime < 8))
                            break;
                        k++;
                    }

                    if (k < times[j].length) {
                        trip.times[j] = times[j][k];
                        lastStopTime = trip.times[j];
                    }
                    else
                        trip.times[j] = -1;

                    times[j][k] = -1;
                }

                let j = 0;
                while (trip.times[j] == -1)
                    j++;

                trip.start = trip.times[j];
                trip.end = lastStopTime;
                trip.duration = trip.end - trip.start;

                if (trip.times.includes(-1))
                    continue;

                logicalNetwork.trips.push(trip);
            }

            let error = false;
            for (let t of times) {
                for (let r of t) {
                    if (r != -1)
                        error = true;
                }
            }

            if (error) {
                console.log("TRIP ERROR ");
                console.log(route);
            }
        }
    }
};

function processRoute(physicalNetwork, scheduleRoute, startNodeId, endRouteNodeName) { //finds route's path
    const route = {
        nodes: [], //list of nodes that the route goes through
        stops: [], //stop nodes
        scheduleRoute: scheduleRoute //reference to route data in schedule
    };

    const initialNode = physicalNetwork.nodes.get(startNodeId);
    route.nodes.push(initialNode.id);

    let currStop = initialNode;
    route.stops.push(initialNode.id);

    for (let i = 1; i < scheduleRoute.stops.length; i++) {
        const nextStop = scheduleRoute.stops[i].name;

        let next2Stop;
        if (i != scheduleRoute.stops.length - 1)
            next2Stop = scheduleRoute.stops[i + 1].name;
        else
            next2Stop = endRouteNodeName;

        let path;
        const target1 = [...physicalNetwork.stopsIds.get(nextStop)];

        while (target1.length != 0) {
            const [dis1, path1] = findPath(currStop, target1);
            const intermediatePlatform = physicalNetwork.nodes.get(path1[0]);

            const [dis2, path2] = findPath(intermediatePlatform, physicalNetwork.stopsIds.get(next2Stop));

            const endPlatform = physicalNetwork.nodes.get(path2[0]);
            const physicalDistance = Math.sqrt((endPlatform.x - intermediatePlatform.x) ** 2 + (endPlatform.y - intermediatePlatform.y) ** 2);

            if (dis2 < 2 * physicalDistance) {
                path = path1;
                break;
            }
            else {
                for (let i = target1.length - 1; i != -1; i--) {
                    const n = target1[i];
                    if (n == path1[0])
                        target1.splice(i, 1);
                }
            }
        }
        route.nodes = path.concat(route.nodes);
        path.push(currStop.id);
        currStop = physicalNetwork.nodes.get(path[0]);

        route.stops.push(currStop.id);
    }

    return route;
}