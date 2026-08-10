export type Chip8Module = {
  HEAPU8: Uint8Array
  HEAPU32: Uint32Array
  _reset: () => void
  _runFrame: () => void
  _getVideoBuffer: () => number
  _malloc: (size: number) => number
  _free: (ptr: number) => void
  _loadROM: (ptr: number, size: number) => boolean
  _setKey: (key: number, pressed: number) => void
  locateFile?: (path: string) => string
  onRuntimeInitialized?: () => void
}

declare global {
  interface Window {
    Module?: Chip8Module
  }
}

let modulePromise: Promise<Chip8Module> | null = null

export function loadChip8Module(): Promise<Chip8Module> {
  if (modulePromise) {
    return modulePromise
  }

  modulePromise = new Promise((resolve, reject) => {
    const existingModule = window.Module

    if (existingModule?._runFrame) {
      resolve(existingModule)
      return
    }

    window.Module = {
      ...(existingModule ?? {}),
      locateFile: (path: string) => `/${path}`,
      onRuntimeInitialized: () => {
        if (window.Module) {
          resolve(window.Module)
        } else {
          reject(new Error('CHIP-8 WASM module did not initialize'))
        }
      },
    } as Chip8Module

    const script = document.createElement('script')
    script.src = '/chip8.js'
    script.async = true
    script.onerror = () => reject(new Error('Failed to load /chip8.js'))
    document.body.appendChild(script)
  })

  return modulePromise
}