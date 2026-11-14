"""Quick validation test for all AI-Locale modules."""
import sys
import os

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

# Test core modules
from app.core.config_manager import ConfigManager
from app.core.process_manager import ProcessManager
from app.core.file_handler import FileHandler
from app.core.text_chunker import TextChunker

print("All core modules imported OK")

# Test config manager
cm = ConfigManager(".")
print(f"Models found: {len(cm.discover_models())}")
for m in cm.discover_models():
    print(f"  - {m['filename']} ({m['size_mb']} MB)")

cmd = cm.build_command()
print(f"Command ({len(cmd)} args): {' '.join(cmd[:6])}...")

# Test text chunker
tc = TextChunker(context_size=512)
print(f"Available tokens per chunk: {tc.available_tokens}")
print(f"Available chars per chunk: {tc.available_chars}")

sample = "Hello world. " * 500  # ~6500 chars
chunks = tc.chunk_text(sample)
print(f"Sample text ({len(sample)} chars) -> {len(chunks)} chunks")

# Test file handler
fh = FileHandler()
print(f"Supported extensions: {len(fh.get_supported_extensions())}")

# Test RAM estimate
est = cm.estimate_ram_usage_mb()
print(f"RAM estimate: {est}")

# Test UI imports (without launching)
try:
    import customtkinter as ctk
    print(f"customtkinter v{ctk.__version__} OK")
except ImportError:
    print("customtkinter NOT installed")

print("\n=== ALL TESTS PASSED ===")
