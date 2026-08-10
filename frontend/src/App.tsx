import { useEffect, useRef, useState } from 'react'
import { loadChip8Module } from './emscripten'
import type { Chip8Module } from './emscripten';
import './App.css'

const WIDTH = 64
const HEIGHT = 32
const SCALE = 10

function App() {
  const canvasRef = useRef<HTMLCanvasElement>(null)
  const [status, setStatus] = useState("Loading WASM")
  const [running, setRunning] = useState(true)

  const keyMap: Record<string, number> = {
    '1': 0x1,
    '2': 0x2,
    '3': 0x3,
    '4': 0xC,

    q: 0x4,
    w: 0x5,
    e: 0x6,
    r: 0xD,

    a: 0x7,
    s: 0x8,
    d: 0x9,
    f: 0xE,

    z: 0xA,
    x: 0x0,
    c: 0xB,
    v: 0xF,
  }

  useEffect(() => {
    let animationId = 0
    let cancelled = false
    let handleKeyDown: ((event: KeyboardEvent) => void) | null = null
    let handleKeyUp: ((event: KeyboardEvent) => void) | null = null

    async function start() {
      const chip8 = await loadChip8Module()
      await loadROM(chip8, '/roms/test_opcode.ch8')

      if (cancelled) return

      handleKeyDown = (event: KeyboardEvent) => {
        const key = keyMap[event.key.toLowerCase()]

        if (key !== undefined) {
          chip8._setKey(key, 1)
        }
      }

      handleKeyUp = (event: KeyboardEvent) => {
        const key = keyMap[event.key.toLowerCase()]

        if (key !== undefined) {
          chip8._setKey(key, 0)
        }
      }

      window.addEventListener('keydown', handleKeyDown)
      window.addEventListener('keyup', handleKeyUp)

      const canvas = canvasRef.current

      if (!canvas) {
        setStatus('Canvas unavailable')
        return
      }

      const ctx = canvas?.getContext('2d')

      if (!ctx) {
        setStatus('Canvas unavailable')
        return 
      }

      chip8._reset()
      setStatus('Running')

      const context = ctx
      function loop() {
        if (running) {
          chip8._runFrame()
        }

        const videoPtr = chip8._getVideoBuffer()
        const video = chip8.HEAPU32.subarray(
          videoPtr / 4,
          videoPtr / 4 + WIDTH * HEIGHT
        )

        context.fillStyle = 'black'
        context.fillRect(0, 0, WIDTH * SCALE, HEIGHT * SCALE)

        context.fillStyle = 'white'

        for (let y = 0; y < HEIGHT; y++) {
          for (let x = 0; x < WIDTH; x++) {
            const pixel = video[y * WIDTH + x]

            if (pixel !== 0) {
              context.fillRect(x * SCALE, y * SCALE, SCALE, SCALE)
            }
          }
        }

        animationId = requestAnimationFrame(loop)
      }

      loop()
    }

    // async function loadROM(chip8: Chip8Module, url: string) {
    //   const response = await fetch(url)
    //   const rom = new Uint8Array(await response.arrayBuffer())

    //   const ptr = chip8._malloc(rom.length)

    //   try {
    //     chip8.HEAPU8.set(rom, ptr)

    //     const ok = chip8._loadROM(ptr, rom.length)

    //     if (!ok) {
    //       throw new Error('ROM failed to load')
    //     }
    //   } finally {
    //     chip8._free(ptr)
    //   }
    // }

    start()

    return () => {
      cancelled = true
      cancelAnimationFrame(animationId)

      if (handleKeyDown) {
        window.removeEventListener('keydown', handleKeyDown)
      }

      if (handleKeyUp) {
        window.removeEventListener('keyup', handleKeyUp)
      }
    }
  }, [])

  async function loadROM(chip8: Chip8Module, url: string) {
      const response = await fetch(url)
      const rom = new Uint8Array(await response.arrayBuffer())

      const ptr = chip8._malloc(rom.length)

      try {
        chip8.HEAPU8.set(rom, ptr)

        const ok = chip8._loadROM(ptr, rom.length)

        if (!ok) {
          throw new Error('ROM failed to load')
        }
      } finally {
        chip8._free(ptr)
      }
  }

  async function resetROM() {
    const chip8 = await loadChip8Module()
    await loadROM(chip8, '/roms/test_opcode.ch8')
  }

  return (
    <main>
      <h1>CHIP-8</h1>
      <p>{status}</p>
      <button type="button" onClick={() => setRunning((value) => !value)}>
        {running ? 'Pause' : 'Run'}
      </button>
      <button type="button" onClick={resetROM}>
        Reset
      </button>

      <canvas
        ref={canvasRef}
        width={WIDTH * SCALE}
        height={HEIGHT * SCALE}
      />
    </main>
  )
}

export default App
