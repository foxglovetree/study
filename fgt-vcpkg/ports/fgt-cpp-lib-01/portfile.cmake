vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO foxglovetree/cpp-lib-01          # 你的 GitHub 用户名/仓库名
    REF v0.2.0                     # Git tag 或 commit hash
    SHA512 000000...               # 稍后生成
    HEAD_REF main                  # 默认分支（用于计算 SHA512）
)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
)

vcpkg_cmake_install()
vcpkg_cmake_config_fixup()

vcpkg_copy_pdbs()

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")
vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE")