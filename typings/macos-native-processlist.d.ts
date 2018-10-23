declare module 'macos-native-processlist' {
  export interface Process {
    pid: number
    ppid: number
    pgid: number
    name: string
    path: string
    children: Process[]
  }

  export function getProcessList (): Promise<Process[]>
}
