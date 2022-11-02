from pathlib import Path


# paths
root_path  = Path(__file__).parent
start_path = root_path / 'Test' / 'CoreTest'


# output files
includes        = start_path / 'includes.txt'
project_sources = start_path / 'project_sources.txt'



# headers
h_files   = list(start_path.glob('**/*.h'))
h_files.sort()

# sources
cpp_files = list(start_path.glob('**/*.cpp'))
cpp_files.sort()

# qt resource files
qrc_files = list(start_path.glob('**/*.qrc'))
qrc_files.sort()

# qt user interface files
ui_files  = list(start_path.glob('**/*.ui'))
ui_files.sort()

# generate project_sources
files = [*h_files, *cpp_files, *qrc_files, *ui_files]
files = [file.relative_to(start_path) for file in files]
files.sort()
project_sources.open('w').write('\n'.join(map(str, files)))

# generate includes
dirs = []
for h_file in h_files:
    dir = h_file.parent
    if dir not in dirs:
        dirs.append(dir)
dirs = [dir.relative_to(start_path) for dir in dirs]
dirs.sort()
includes.open('w').write('\n'.join(map(str, dirs)))
