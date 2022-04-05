# macos-native-processlist

[![Build](https://github.com/Eugeny/macos-native-processlist/actions/workflows/main.yml/badge.svg)](https://github.com/Eugeny/macos-native-processlist/actions/workflows/main.yml)

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
