import { useEffect, useRef, useState } from 'react'
import { loadChip8Module } from './emscripten'
import './App.css'

const WIDTH = 64
const HEIGHT = 32
const SCALE = 10

function App() {
  const canvasRef = useRef<HTMLCanvasElement>(null)
  const [status, setStatus] = useState("Loading WASM")

  useEffect(() => {
    let animationId = 0
    let cancelled = false

    async function start() {
      const chip8 = await loadChip8Module()

      if (cancelled) return

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
        chip8._runFrame()

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

    start()

    return () => {
      cancelled = true
      cancelAnimationFrame(animationId)
    }
  }, [])

  return (
    <main>
      <h1>CHIP-8</h1>
      <p>{status}</p>

      <canvas
        ref={canvasRef}
        width={WIDTH * SCALE}
        height={HEIGHT * SCALE}
      />
    </main>
  )
}

export default App
