const native = require('./build/Release/native.node')

module.exports = {
  getProcessList: function () {
    return new Promise(resolve => {
      native.getProcessList(processes => {
        for (let process of processes) {
          process.children = processes.filter(x => x.ppid === process.pid)
        }

        resolve(processes)
      })
    })
  }
}
