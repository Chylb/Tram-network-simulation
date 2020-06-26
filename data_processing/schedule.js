const findPath = require("./graph.js").findPath;

module.exports = {
    createRoutes: function (physicalNetwork, schedule, logicalNetwork) {
        routeStart = new Map();
        routeEnd = new Map();
        
        routeStart.set("Wzgórza Krzesławickie - Salwator", 213578451);
        routeEnd.set("Wzgórza Krzesławickie - Salwator", 2419732952);
        routeStart.set("Salwator - Wzgórza Krzesławickie", 2419732952);
        routeEnd.set("Salwator - Wzgórza Krzesławickie", 2426087293);

        routeStart.set("Salwator - Cmentarz Rakowicki", 2419732952);
        routeEnd.set("Salwator - Cmentarz Rakowicki", 2425718627);
        routeStart.set("Cmentarz Rakowicki - Salwator", 321458378);
        routeEnd.set("Cmentarz Rakowicki - Salwator", 2419732952);

        routeStart.set("Nowy Bieżanów P+R - Krowodrza Górka", 287586332);
        routeEnd.set("Nowy Bieżanów P+R - Krowodrza Górka", 257656078);
        routeStart.set("Krowodrza Górka - Nowy Bieżanów P+R", 257655889);
        routeEnd.set("Krowodrza Górka - Nowy Bieżanów P+R", 623562103);

        routeStart.set("Wzgórza Krzesławickie - Bronowice Małe", 213578451);
        routeEnd.set("Wzgórza Krzesławickie - Bronowice Małe", 2419986541);
        routeStart.set("Bronowice Małe - Wzgórza Krzesławickie", 2419986540);
        routeEnd.set("Bronowice Małe - Wzgórza Krzesławickie", 2426087293);

        routeStart.set("TAURON Arena Kraków Wieczysta - Krowodrza Górka", 4069652993);
        routeEnd.set("TAURON Arena Kraków Wieczysta - Krowodrza Górka", 2423851561);
        routeStart.set("Krowodrza Górka - TAURON Arena Kraków Wieczysta", 257655889);
        routeEnd.set("Krowodrza Górka - TAURON Arena Kraków Wieczysta", 639603980);

        routeStart.set("Borek Fałęcki - Bronowice Małe", 321429867);
        routeEnd.set("Borek Fałęcki - Bronowice Małe", 2419986541);
        routeStart.set("Bronowice Małe - Borek Fałęcki", 2419986540);
        routeEnd.set("Bronowice Małe - Borek Fałęcki", 321429856);

        routeStart.set("Bardosa - Nowy Bieżanów P+R", 2194801607);
        routeEnd.set("Bardosa - Nowy Bieżanów P+R", 287442350);
        routeStart.set("Nowy Bieżanów P+R - Bardosa", 5322817078);
        routeEnd.set("Nowy Bieżanów P+R - Bardosa", 773635753);

        routeStart.set("Mały Płaszów P+R - Czerwone Maki P+R", 1889519517);
        routeEnd.set("Mały Płaszów P+R - Czerwone Maki P+R", 2017955948);
        routeStart.set("Czerwone Maki P+R - Mały Płaszów P+R", 2017955951);
        routeEnd.set("Czerwone Maki P+R - Mały Płaszów P+R", 2431520659);

        routeStart.set("Nowy Bieżanów P+R - Bronowice", 5322817078);
        routeEnd.set("Nowy Bieżanów P+R - Bronowice", 2428098333);
        routeStart.set("Bronowice - Nowy Bieżanów P+R", 2419959764);
        routeEnd.set("Bronowice - Nowy Bieżanów P+R", 287442350);

        routeStart.set("Pleszów - Bronowice", 1764757832);
        routeEnd.set("Pleszów - Bronowice", 2428098333);
        routeStart.set("Bronowice - Pleszów", 2419959764);
        routeEnd.set("Bronowice - Pleszów", 2427003514);

        routeStart.set("Czerwone Maki P+R - Krowodrza Górka", 2017955951);
        routeEnd.set("Czerwone Maki P+R - Krowodrza Górka", 2423851561);
        routeStart.set("Krowodrza Górka - Czerwone Maki P+R", 257655889);
        routeEnd.set("Krowodrza Górka - Czerwone Maki P+R", 2017955948);
        
        routeStart.set("Borek Fałęcki - Dworzec Towarowy", 321429867);
        routeEnd.set("Borek Fałęcki - Dworzec Towarowy", 2420286330);
        routeStart.set("Dworzec Towarowy - Borek Fałęcki", 2420286329);
        routeEnd.set("Dworzec Towarowy - Borek Fałęcki", 321429856);
        
        routeStart.set("Mały Płaszów P+R - Cichy Kącik", 1889519517);
        routeEnd.set("Mały Płaszów P+R - Cichy Kącik", 2423298435);
        routeStart.set("Cichy Kącik - Mały Płaszów P+R", 2419831874);
        routeEnd.set("Cichy Kącik - Mały Płaszów P+R", 2431520659);

        routeStart.set("Borek Fałęcki - Kopiec Wandy", 321429867);
        routeEnd.set("Borek Fałęcki - Kopiec Wandy", 213578731);
        routeStart.set("Kopiec Wandy - Borek Fałęcki", 2426893478);
        routeEnd.set("Kopiec Wandy - Borek Fałęcki", 321429856);
        
        routeStart.set("Kurdwanów P+R - Bronowice Małe", 5728702340);
        routeEnd.set("Kurdwanów P+R - Bronowice Małe", 2419986541);
        routeStart.set("Bronowice Małe - Kurdwanów P+R", 2419986540);
        routeEnd.set("Bronowice Małe - Kurdwanów P+R", 289685340);
        
        routeStart.set("Krowodrza Górka - Kurdwanów P+R", 257655889);
        routeEnd.set("Krowodrza Górka - Kurdwanów P+R", 289685340);
        routeStart.set("Kurdwanów P+R - Krowodrza Górka", 5728702340);
        routeEnd.set("Kurdwanów P+R - Krowodrza Górka", 2423851561);
        
        routeStart.set("Czerwone Maki P+R - Wzgórza Krzesławickie", 2017955951);
        routeEnd.set("Czerwone Maki P+R - Wzgórza Krzesławickie", 2426087293);
        routeStart.set("Wzgórza Krzesławickie - Czerwone Maki P+R", 213578451);
        routeEnd.set("Wzgórza Krzesławickie - Czerwone Maki P+R", 2017955948);
        
        let id = 0;

        for (let line of schedule.lines) {
            dir1Start = routeStart.get(line.direction1.name);
            dir1End = routeEnd.get(line.direction1.name);

            dir2Start = routeStart.get(line.direction2.name);
            dir2End = routeEnd.get(line.direction2.name);

            let route1;
            let route2;

            if (dir1Start != undefined) {
                route1 = processRoute(physicalNetwork, line.direction1, dir1Start, dir1End);
                route1.id = id;
                id++;
            }
            if (dir2Start != undefined) {
                route2 = processRoute(physicalNetwork, line.direction2, dir2Start, dir2End);
                route2.id = id;
                id++;
            }

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

                    const ms = stop.schedule[j]["Dzień powszedni"].split(" ");

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

function processRoute(physicalNetwork, scheduleRoute, startNodeId, endNodeId) { //finds route's path
    const route = {
        nodes: [], //list of nodes that the route goes through
        stops: [], //stop nodes
        scheduleRoute: scheduleRoute //reference to route data in schedule
    };

    const initialNode = physicalNetwork.nodes.get(startNodeId);
    route.nodes.push(initialNode.id);

    let currStop = initialNode;
    route.stops.push(initialNode.id);

    for (let i = 1; i < scheduleRoute.stops.length - 1; i++) {
        const nextStop = scheduleRoute.stops[i].name;
        const next2Stop = scheduleRoute.stops[i + 1].name;

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

    const [dis, lastPath] = findPath(currStop, [endNodeId]);
    route.nodes = lastPath.concat(route.nodes);
    route.nodes.reverse();
    route.stops.push(endNodeId);

    return route;
}