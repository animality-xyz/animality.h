const binding = require("./build/Release/animality_node");
const { Worker, isMainThread, parentPort } = require("worker_threads");

if (isMainThread)
    module.exports = { blocking: () => binding, ...Object.fromEntries(Object.entries(binding).map(([k, v]) => [k, () => new Promise(resolve => {
        const worker = new Worker(__filename);
        worker.postMessage(k);
        worker.once("message", resolve);
    })]))};
else
    parentPort.once("message", v => {
        const output = binding[v]();
        parentPort.postMessage(output);
        parentPort.close();
    });
