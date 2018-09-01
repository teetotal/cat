const fs = require('fs');
const titles = require('./actions_titles');
var id=1;
var itemId = 1;
var actions = {
    actions:[

    ]
};
var types = ["s", "i", "a", "si", "sa", "ia", "sia"];

function createProperty(level, type)
{
    var valueReward = level * 8;
    var S = 0;
    var I = 0;
    var A = 0;
    var point = 0;
    switch(types[type]){
        case "s":
        S = valueReward;
        break;
        case "i":
        I = valueReward;
        break;
        case "a":
        A = valueReward;
        break;
        case "si":
        S = valueReward / 2;
        I = valueReward / 2;
        break;
        case "sa":
        S = valueReward / 2;
        A = valueReward / 2;
        break;
        case "ia":
        I = valueReward / 2;
        A = valueReward / 2;
        break;
        case "sia":
        S = valueReward / 3;
        A = valueReward / 3;
        I = valueReward / 3;
        break;
    }
    var property = {
        "strength": Math.floor(S),
        "intelligence": Math.floor(I),
        "appeal": Math.floor(A),
        "point": point,
        "items": []
    };

    return property;
}

function getTitle(level, type){
    return titles[types[type]][level-1];
}

function createAction(level, type){
    if(getTitle(level, type) == null)
        return;

    var obj = {
        "id": id++,
        "name": getTitle(level, type),
        "type": type,
        "level": level,
        "reward": createProperty(level, type),
        "cost": {
          "strength": 0,
          "intelligence": 0,
          "appeal": 0,
          "point": 0,
          "items": [
            {
              "id": type + 1,
              "quantity": level
            }
          ]
        }
    };
    actions.actions.push(obj);
}
function create(level){
    for(var n=0; n < ((level < 4) ? 4 : types.length); n++){
        createAction(level, n);
    }
}

//level
for(var level =1; level <= 12; level++){
    create(level);
}
fs.writeFile("../Resources/actions.json", JSON.stringify(actions, '', '\t'));
console.log(actions);
