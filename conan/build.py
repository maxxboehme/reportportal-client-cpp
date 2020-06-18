from cpt.packager import ConanMultiPackager


if __name__ == "__main__":
    builder = ConanMultiPackager(
        archs="x86_64",
        build_policy="missing")
    builder.add_common_builds(shared_option_name=False)
    builder.run()
