import QtQuick 1.1

Item {

    Component {
      id:stripe
      Rectangle { color: "white"; }
    }

    id: container
    //property variant stripe_properties;

    property list<Component> stripes

    function addStripe(atPixel, properties) {
        if (stripes[atPixel] !== undefined) {
            console.log("stripe already existed");
            return true;
        }

        //if (stripe.status != Component.Ready) {
        //    console.log("stripes not ready");
        //    return false;
        //}
        var prop = {"x": atPixel, "y": 0, "height": container.height, "width": 1};
        for (var p in properties) { prop[p] = properties[p]; }
        stripes[atPixel] = stripe.createObject(container, prop);
    }
}
