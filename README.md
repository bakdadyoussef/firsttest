# AI-Locale — Local AI Chatbot & Document Q&A Interface

A lightweight, modern Python GUI designed specifically to run local AI models on low-spec hardware (**4 GB RAM**, **Intel i3 3rd gen**, **HDD**).

---

## 🚀 How to Launch

Simply double-click:
```bat
launch.bat
```
Or run from your terminal:
```bash
python app/main.py
```

---

## ✨ Features Built

1. **Subprocess Engine (`llama-cli.exe`)**:
   - Zero-RAM overhead from Python (the model runs inside llama.cpp, not inside Python).
   - Real-time token streaming into the chat window.
   - Graceful start / stop / restart controls.

2. **Customizable Settings Panel**:
   - **Model Selector**: Auto-detects all `.gguf` models in the `models/` directory (Qwen 1.5B and Qwen 3.5 4B).
   - **Hardware Presets**:
     - `Safe (1.5B, 512 ctx)`: Minimal RAM, fast startup, zero swapping.
     - `Balanced (1.5B, 2048 ctx)`: Larger context for longer conversations.
     - `Maximum (4B, 512 ctx)`: High intelligence model, safe memory configuration.
     - `4B Extended (4B, 1024 ctx)`: Extended context for 4B model.
   - **Performance Tuning**: Adjust Context Size, Batch Size, Micro-Batch Size, and CPU Threads (1-4).
   - **KV Cache Quantization**: Select `q4_0` / `q8_0` / `f16` to halve KV cache memory usage on your 4 GB RAM.
   - **Sampling Parameters**: Live sliders for Temperature, Top-K, Top-P, Min-P, and Repeat Penalty.
   - **System Prompt**: Edit and persist custom system personas.
   - **Live RAM Estimator**: Displays predicted resident RAM consumption and warns if swapping risk is high.

3. **File Feeding & Context-Aware Document Q&A**:
   - Supports 38+ file types: `.txt`, `.md`, `.csv`, `.json`, `.log`, `.py`, `.html`, `.sql`, etc.
   - **Smart Text Chunker**: Automatically slices long documents into context-sized pieces based on paragraph/sentence boundaries.
   - **Chunk Navigator**: Browse through document chunks (`◀ Prev` / `Next ▶`) and ask questions tailored to specific chunks.
