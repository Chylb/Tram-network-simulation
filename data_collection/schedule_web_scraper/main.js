const request = require('request');
const cheerio = require('cheerio');
const fs = require('fs');

const HtmlTableToJson = require('html-table-to-json');

const strona = 'http://rozklady.mpk.krakow.pl';

async function main() {
    try {
        let mainBody = await getQuote('http://rozklady.mpk.krakow.pl/?lang=PL&akcja=index&rozklad=20200323');
        let $ = cheerio.load(mainBody);

        const rozklad = { lines: [] };

        const numeryLinii = [];
        const linkiLinii = [];

        $('.linia_table_left a').each((i, el) => {
            const text = $(el).text();
            const link = $(el).attr('href');

            if (parseInt(text) < 53) {
                numeryLinii.push(text);
                linkiLinii.push(link);
            }
        });

        for (let i = 0; i < linkiLinii.length; i++) {
            const link = linkiLinii[i];
            const numer = numeryLinii[i];
            const body = await getQuote(strona + link);
            const $linia = cheerio.load(body);

            const linkiKierunkow = [];
            const nazwyKierunkow = [];

            $linia('td table tr td table tr td table tr td a').each((i, el) => {
                linkiKierunkow.push($linia(el).attr('href'));
                nazwyKierunkow.push($linia(el).text());
            });

            const kierunek1 = await przetworzKierunek(strona + linkiKierunkow[0], nazwyKierunkow[0]);
            const kierunek2 = await przetworzKierunek(strona + linkiKierunkow[1], nazwyKierunkow[1]);

            const linia = {
                number: numer,
                direction1: kierunek1,
                direction2: kierunek2
            }

            rozklad.lines.push(linia);
        }

        fs.writeFileSync('./schedule.json', JSON.stringify(rozklad));
        console.log("Koniec");
    }
    catch (error) {
        console.error(error);
    }

}
main();

async function przetworzKierunek(link, nazwa) {
    try {
        const body = await getQuote(link);
        const $linia = cheerio.load(body);

        const linkiPrzystankow = [];

        $linia('a span').each((i, el) => {
            linkiPrzystankow.push($linia(el).parent().attr('href'));
        });

        const kierunek = {
            name: nazwa,
            stops: []
        };

        for (let linkPrzystanku of linkiPrzystankow) {
            const bodyPrzystanku = await getQuote(strona + linkPrzystanku);
            const $przystanek = cheerio.load(bodyPrzystanku);

            let nazwaPrzystanku;
            $przystanek('div span').each((i, el) => {
                if (i == 0) {
                    nazwaPrzystanku = $przystanek(el).text();
                }
            });

            const rozkladPrzystanku = $przystanek('tr td table')[14];

            let tabelaHtml = $przystanek(rozkladPrzystanku).html();
            tabelaHtml = "<table>" + tabelaHtml + "</table>";
            const tabelaJson = HtmlTableToJson.parse(tabelaHtml);

            const przystanek = {
                name: nazwaPrzystanku,
                schedule: tabelaJson.results[0]
            };

            kierunek.stops.push(przystanek);
        };

        return kierunek;
    }
    catch (error) {
        console.error(error);
        return error;
    }
}

function getQuote(url) {
    const headers = {
        'Host': 'rozklady.mpk.krakow.pl',
        'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/80.0.3987.149 Safari/537.36',
        'Accept': 'text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9',
        'Referer': 'http://rozklady.mpk.krakow.pl/',
        'Accept-Encoding': 'gzip, deflate',
        'Accept-Language': 'pl-PL,pl;q=0.9,en-US;q=0.8,en;q=0.7',
        'Cookie': 'ROZKLADY_JEZYK=PL; ROZKLADY_WIDTH=2000; ROZKLADY_AB=0; __utma=174679166.140374276.1585039119.1585039119.1585039119.1; __utmc=174679166; __utmz=174679166.1585039119.1.1.utmcsr=google|utmccn=(organic)|utmcmd=organic|utmctr=(not%20provided); ROZKLADY_WIZYTA=23; ROZKLADY_OSTATNIA=1585093785'
    };

    return new Promise(function (resolve, reject) {
        const fsUrl = './pages/' + url.replace(/[/\\?%*:|"<>]/g, '-');

        try {
            if (fs.existsSync(fsUrl)) {
                fs.readFile(fsUrl, function (err, data) {
                    if (err) return reject(err);
                    resolve(data);
                });
            }
            else {
                console.log("odwiedzam " + url);
                request({
                    url: url,
                    rejectUnauthorized: false,
                    headers: headers
                },
                    function (error, response, body) {
                        console.log(" O NIE");
                        fs.writeFileSync(fsUrl, body);
                        if (error) return reject(error);
                        resolve(body);
                    });
            }
        } catch (err) {
            console.error(err)
            return reject(err);
        }
    });
}





