"""CPU-only verify retained source hashes and every preserved report payload."""
import gzip
import hashlib
import json
from pathlib import Path

r=Path(__file__).resolve().parent
versions=[]
for manifest in sorted((r/'versions').glob('*/manifest.json')):
    d=json.loads(manifest.read_text(encoding='utf-8'))
    for f,h in d['source_sha256'].items():
        assert hashlib.sha256((manifest.parent/'src'/f).read_bytes()).hexdigest()==h, str(manifest)+':'+f
    for record in d['variants'].values():assert (manifest.parent/'src'/record['entry']).is_file()
    versions.append(manifest.parent.name)
reports=json.loads((r/'records'/'PRESERVATION_MANIFEST.json').read_text(encoding='utf-8'))
for row in reports['files']:
    path=r/'records'/row['stored_path']
    with (gzip.open(path,'rb') if row['gzip'] else path.open('rb')) as f:
        h=hashlib.sha256()
        for block in iter(lambda:f.read(4*1024*1024),b''):h.update(block)
    assert h.hexdigest()==row['sha256'],row['original_path']
print(json.dumps({'versions':versions,'preserved_reports':len(reports['files']),
                  'all_source_and_report_hashes_verified':True},indent=2))
