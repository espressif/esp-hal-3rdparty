#!/bin/bash

set -ex

# If the pipeline is running from a branch different from project's default
# add a suffix to push sync branch
if [ "${CI_COMMIT_BRANCH}" != "${CI_DEFAULT_BRANCH}" ]; then
    DEBUG_SUFFIX="-debug"
fi

# Usage: clone_idf ESP_IDF_BRANCH
clone_idf() {
    git clone --single-branch --branch "$1" "${IDF_URL}" .
}

# Usage: find_pattern STRING
find_pattern() {
    git --no-pager log -i --grep "$1" > issue_found.txt
    if [[ $(cat issue_found.txt | wc -l) > 0 ]]; then
        ISSUE_NUM=$(grep "^commit " issue_found.txt | wc -l)
        die "${ISSUE_NUM} $1 found. See issue_found.txt"
    fi
}

check_links() {
    find_pattern 'github.com/[^ /]*/[^ /]*/\(issues\|pull\)'
    find_pattern 'espressif/esp-idf[!#$&~%^]'
}

# Usage: extract_components ESP_IDF_BRANCH SYNC_BRANCH_NAME ARGS...
extract_components() {
    ESP_IDF_BRANCH=$1
    SYNC_BRANCH_NAME=$2${DEBUG_SUFFIX}
    ARGS="${@:3}"

    FOLDER_NAME="esp-idf/${SYNC_BRANCH_NAME}"

    rm -rf ${FOLDER_NAME}
    mkdir -p ${FOLDER_NAME}

    pushd ${FOLDER_NAME}

    echo "Cloning ESP-IDF (${ESP_IDF_BRANCH})"

    clone_idf "${ESP_IDF_BRANCH}"

    echo "Extract to branch ${SYNC_BRANCH_NAME} with arg list: '$ARGS'"

    git filter-repo "${@:3}"

    check_links

    git checkout -B ${SYNC_BRANCH_NAME}
    git push ${ESP_HAL_3RDPARTY_URL} ${SYNC_BRANCH_NAME} || {
        push_to_temporary_branch ${ESP_HAL_3RDPARTY_URL} ${SYNC_BRANCH_NAME}
        force_push_job "${SYNC_BRANCH_NAME}" >> ../force_push.yml
    }
    git clean -xdff
    popd
}

# Create a temporary branch to store the branch to be pushed
# by the child pipeline.
push_to_temporary_branch() {
    TEMP_BRANCH_SUFFIX="-$(mktemp -u XXXXXX)"

    git branch -D ${2}${TEMP_BRANCH_SUFFIX} || true
    git checkout -b ${2}${TEMP_BRANCH_SUFFIX}
    git push ${1} ${2}${TEMP_BRANCH_SUFFIX}
}

# Create yaml to force push the sync branch
force_push_job() {
    SET_RUN_FORCE_PUSH="true"

    echo "force_push_${1}:"
    echo "  stage: Force Push"
    echo "  allow_failure: true"
    echo "  when: manual"
    echo "  variables:"
    echo "    GIT_DEPTH: 0"
    echo "    CHILD_ESP_HAL_3RDPARTY_URL: https://gitlab-ci-token:\${CI_ESP_HAL_3RDPARTY_TOKEN}@\${CI_SERVER_HOST}:\${CI_SERVER_PORT}/\${CI_PROJECT_PATH}.git"
    echo "  script:"
    echo "    - |"
    echo "      git fetch \${CHILD_ESP_HAL_3RDPARTY_URL} ${1}${TEMP_BRANCH_SUFFIX}"
    echo "      git checkout ${1}${TEMP_BRANCH_SUFFIX}"
    echo "      git branch -D ${1} || true"
    echo "      git checkout -b ${1}"
    echo "      git push --force \${CHILD_ESP_HAL_3RDPARTY_URL} ${1}"
    echo "      git push \${CHILD_ESP_HAL_3RDPARTY_URL} --delete ${1}${TEMP_BRANCH_SUFFIX}"
    echo ""
}

# This creates the initial configuration for the pipeline.
mkpipeline() {
    echo "variables:"
    echo "  GIT_STRATEGY: clone"
    echo ""
    echo "default:"
    echo "  image: \$CI_DOCKER_REGISTRY/nuttx/target-test-env-v5.1"
    echo "  tags:"
    echo "    - build"
    echo ""
    echo "stages:"
    echo "  - Force Push"
    echo ""
}

# This creates a null job in the case of forcing-push isn't being required
nulljob() {
    echo "no_force_push_required:"
    echo "  stage: Force Push"
    echo "  script:"
    echo "    - echo \"No force push required!\""
    echo ""
}

# Usage get_arg_by_components [COMPONENTS...]
get_arg_by_components() {
    RET=""
    for COMPONENT in $*
    do
        # There will be an redundant trailing space
        RET+="--path components/${COMPONENT} "
    done
    echo ${RET}
}

############## Conditional Pipeline ###################

# During the development of the sync branches, it may be necessary to force push-some of them to the
# upstream. This action, however, isn't recommended to be performed automatically. Therefore, we can
# make use of the dynamically-created child pipelines to create a manually-triggered job to
# force-push the sync branch.

mkpipeline > force_push.yml

LIC_ARG="--path LICENSE"

# The commits have the same SHA as long as the commit author, date, message and change list are the same.
# Any modification to the strategy will create new branch that cannot be merged (pushed) to the existing one.

MSG_CALLBACK="
        # (https)?github.com/[user]/[proj]/*/[id] -> [Github: [user]/proj]/*/[id]
        msg1 = re.sub(br'(?:(?:https?://)?github\.com/)([^\s/]+/[^\s/]+)/([^/]+/[0-9]+)', br'[Github: \1]/\2', message)

        # espressif/esp-idf -> [ESP_IDF] (except from [Github: espressif/esp-idf])
        return re.sub(br'espressif/esp-idf([^\]])', br'[ESP_IDF]\1', msg1)
    "

ARG=$(cat << EOF
      ${LIC_ARG} $(get_arg_by_components \
                   bootloader \
                   bootloader_support \
                   bt \
                   efuse \
                   esp_adc \
                   esp_app_format \
                   esp_coex \
                   esp_common \
                   esp_event \
                   esp_hw_support \
                   esp_phy \
                   esp_rom \
                   esp_system \
                   esp_timer \
                   esp_wifi \
                   hal \
                   log \
                   mbedtls \
                   newlib \
                   partition_table \
                   riscv \
                   soc \
                   spi_flash \
                   wpa_supplicant\
                   xtensa \
                   )
EOF
)

extract_components "release/v5.1" "sync/release_v5.1.c" ${ARG} --message-callback "${MSG_CALLBACK}"

# Add new one here if you have new requirement

# Push to protected branch will cause that branch to appear on Github.
# Try with non-protected branch first.
# ARG="${LIC_ARG} $(get_arg_by_components esp_event esp_phy esp_wifi mbedtls wpa_supplicant)"
# extract_components "release/v5.0" "test-sync-1-release_v5.0" ${ARG} --message-callback "${MSG_CALLBACK}"

# If `SET_RUN_FORCE_PUSH` isn't set, add a nulljob to the child pipeline
# to indicate that no force-pushing is required
if [ -z "${SET_RUN_FORCE_PUSH}" ]; then
    nulljob >> force_push.yml
else
    true
fi

############## Deprecated Syncs ###################

# ARG=$(cat << EOF
#       ${LIC_ARG} $(get_arg_by_components \
#                    bootloader_support \
#                    efuse \
#                    esp_app_format \
#                    esp_common \
#                    esp_event \
#                    esp_hw_support \
#                    esp_phy \
#                    esp_rom \
#                    esp_system \
#                    esp_timer \
#                    esp_wifi \
#                    hal \
#                    log \
#                    mbedtls \
#                    newlib \
#                    riscv \
#                    soc \
#                    spi_flash \
#                    wpa_supplicant\
#                    xtensa \
#                    )
# EOF
# )

# extract_components "release/v5.1" "sync-1-release_v5.1" ${ARG} --message-callback "${MSG_CALLBACK}"

# ARG=$(cat << EOF
#       ${LIC_ARG} $(get_arg_by_components \
#                    bootloader_support \
#                    bt \
#                    efuse \
#                    esp_app_format \
#                    esp_common \
#                    esp_event \
#                    esp_hw_support \
#                    esp_phy \
#                    esp_rom \
#                    esp_system \
#                    esp_timer \
#                    esp_wifi \
#                    hal \
#                    log \
#                    mbedtls \
#                    newlib \
#                    riscv \
#                    soc \
#                    spi_flash \
#                    wpa_supplicant\
#                    xtensa \
#                    )
# EOF
# )

# extract_components "release/v5.1" "sync-2-release_v5.1" ${ARG} --message-callback "${MSG_CALLBACK}"

# ARG=$(cat << EOF
#       ${LIC_ARG} $(get_arg_by_components \
#                    bootloader \
#                    bootloader_support \
#                    bt \
#                    efuse \
#                    esp_adc \
#                    esp_app_format \
#                    esp_coex \
#                    esp_common \
#                    esp_event \
#                    esp_hw_support \
#                    esp_phy \
#                    esp_rom \
#                    esp_system \
#                    esp_timer \
#                    esp_wifi \
#                    hal \
#                    log \
#                    mbedtls \
#                    newlib \
#                    riscv \
#                    soc \
#                    spi_flash \
#                    wpa_supplicant\
#                    xtensa \
#                    )
# EOF
# )

# extract_components "release/v5.1" "sync-3-release_v5.1" ${ARG} --message-callback "${MSG_CALLBACK}"

# ARG=$(cat << EOF
#       ${LIC_ARG} $(get_arg_by_components \
#                    bootloader \
#                    bootloader_support \
#                    bt \
#                    efuse \
#                    esp_adc \
#                    esp_app_format \
#                    esp_coex \
#                    esp_common \
#                    esp_event \
#                    esp_hw_support \
#                    esp_phy \
#                    esp_rom \
#                    esp_system \
#                    esp_timer \
#                    esp_wifi \
#                    hal \
#                    log \
#                    mbedtls \
#                    newlib \
#                    riscv \
#                    soc \
#                    spi_flash \
#                    wpa_supplicant\
#                    xtensa \
#                    )
# EOF
# )

# extract_components "release/v5.1" "sync/release_v5.1.b" ${ARG} --message-callback "${MSG_CALLBACK}"
