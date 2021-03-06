# Usage, e.g.:
#   snakemake -s ExtractBoundary.smk -j 1 --configfile $HOME/code/ogs6/build/buildinfo.yaml
#
# buildinfo.yaml contains variables such as Data_BINARY_DIR

output_path = "FileIO"

import os
os.environ["PATH"] += os.pathsep + os.pathsep.join([config['BIN_DIR']])
workdir: f"{config['Data_BINARY_DIR']}/{output_path}"

# "entry point", otherwise one would had to specify output files as snakemake
# arguments
elem_types = ['tri', 'quad']
rule all:
    input:
        expand("square_1x1_{type}_boundary_diff.out", type=elem_types)

rule generate_meshes:
    output:
        "input_square_1x1_{type}.vtu"
    shell:
        """
        generateStructuredMesh -e {wildcards.type} \
            --lx 1 --ly 1 \
            --nx 10 --ny 10 \
            -o {output}
        """

rule extract_boundary:
    input:
        "input_square_1x1_{type}.vtu"
    output:
        "square_1x1_{type}_boundary.vtu"
    shell:
        "ExtractBoundary -i {input} -o {output}"

rule vtkdiff:
    input:
        "square_1x1_{type}_boundary.vtu"
    output:
        "square_1x1_{type}_boundary_diff.out"
    params:
        fields = [
            # second field name can be omitted if identical
            ["bulk_node_ids", 0, 0],
            ["bulk_element_ids", 0, 0],
            ["bulk_face_ids", 0, 0]
        ]
    wrapper:
        f"file://{config['SOURCE_DIR']}/scripts/snakemake/vtkdiff"
