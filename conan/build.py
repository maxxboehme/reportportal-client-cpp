from cpt.packager import ConanMultiPackager


if __name__ == "__main__":
    builder = ConanMultiPackager(options=["reportportal-client-cpp:shared"=False])
    builder.add_common_builds()
    builder.run()
