import { occ } from "..";


const shapeBox = occ.makeBox([1, 2, 3], [20, 30, 40])
const translatedShapeBox = shapeBox.translate(0, 0, 10);
