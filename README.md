# macos-native-processlist

[![Build status](https://ci.appveyor.com/api/projects/status/8uqbavmiq4myr8op?svg=true)](https://ci.appveyor.com/project/Eugeny/macos-native-processlist)

Loads list of processes on macOS natively, without any bullshit `ps` output parsing

## Usage

```js
import { getProcessList } from 'macos-native-processlist'

getProcessList().then(processes => {
  for (let process of processes) {
    console.log(process.pid, process.name, process.children.length)
  }
})
```

For the full API look at the [typings file](./typings/macos-native-processlist.d.ts).
