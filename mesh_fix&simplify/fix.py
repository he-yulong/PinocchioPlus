import pymeshfix

infile = 'cow5804.obj' # incomplete obj file path
outfile = 'output.obj' # fixed file path
tin = pymeshfix.PyTMesh()
tin.load_file(infile)
pymeshfix.clean_from_file(infile, outfile)
