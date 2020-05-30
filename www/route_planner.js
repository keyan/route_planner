var map;
var source;
var target;
var line;

// Hit localhost only when not being served via remote host
var baseUrl = "localhost:4001/";
if (document.location.host) {
    baseUrl = "http://demo.keyanp.com/route?";
}

$(document).ready(function(){
    // Create the map centered in Seattle, restrict bounds to area used
    // for graph construction.
    map = L.map('map_canvas').setView([47.607413, -122.323370], 13);
    var southWest = L.latLng(47.5565, -122.3692);
    var northEast = L.latLng(47.6503, -122.2716);
    map.setMaxBounds(L.latLngBounds(southWest, northEast));

    L.tileLayer('https://api.mapbox.com/styles/v1/{id}/tiles/{z}/{x}/{y}?access_token={accessToken}', {
    attribution: '© <a href="https://www.mapbox.com/about/maps/">Mapbox</a> © <a href="http://www.openstreetmap.org/copyright">OpenStreetMap</a> <strong><a href="https://www.mapbox.com/map-feedback/" target="_blank">Improve this map</a></strong>',
    tileSize: 512,
    maxZoom: 18,
    zoomOffset: -1,
    id: 'mapbox/streets-v11',
    accessToken: 'pk.eyJ1Ijoia3Bpc2hkYWRpYW4iLCJhIjoiY2pvMTVzN2lwMDdrbTNrcG9mb2RvcDF5aSJ9.wbFcMArvH5wSmVwvRomNmg'
    }).addTo(map);

    source = L.marker([47.619872, -122.347477], {draggable: true}).addTo(map);
    target = L.marker([47.598289, -122.334700], {draggable: true}).addTo(map);
    line = L.polyline([], {color: 'red'}).addTo(map);
    routeAndRedraw();

    source.on('dragend', routeAndRedraw);
    target.on('dragend', routeAndRedraw);
});

function routeAndRedraw() {
    var url = baseUrl +
        source.getLatLng().lat + "," + source.getLatLng().lng + "," +
        target.getLatLng().lat + "," + target.getLatLng().lng;
    $.ajax(url, { dataType: "jsonp" });
}

function routingEngineCallback(json) {
    line.remove();
    // See github.com/jieter/Leaflet.encoded
    line = L.polyline(L.PolylineUtil.decode(json.polyline, {precision: 6}), {color: 'red'}).addTo(map);

    target.unbindPopup();
    target
        .bindPopup(`Time to destination: ${json.travel_time}`)
        .openPopup();
}
