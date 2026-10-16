"""Verify archived text/source hashes without HIP, imports, or GPU execution."""
import hashlib
import json
from pathlib import Path

root = Path(__file__).resolve().parent
manifest = json.loads((root / 'ARCHIVE_MANIFEST.json').read_text(encoding='utf-8'))
for item in manifest['files']:
    path = (root / item['path']).resolve()
    if not path.is_relative_to(root) or not path.is_file():
        raise SystemExit(f'Missing/out-of-scope: {item["path"]}')
    data = path.read_bytes()
    if len(data) != item['bytes'] or hashlib.sha256(data).hexdigest() != item['sha256']:
        raise SystemExit(f'Hash mismatch: {item["path"]}')
print(f'PASS: {len(manifest["files"])} archived files; no GPU work')
