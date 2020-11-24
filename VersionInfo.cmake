if (${PINCHOT_API_ROOT_DIR} STREQUAL "")
    message(FATAL_ERROR "PINCHOT_API_ROOT_DIR not defined!")
endif()

if(WIN32)
  set(PARSE_COMMAND python parse-version.py)
elseif(UNIX)
  set(PARSE_COMMAND ./parse-version.py)
else()
  message(FATAL_ERROR "Not building on a supported platform")
endif()

set(SCRIPTS_DIR ${PINCHOT_API_ROOT_DIR}/scripts)

# Stores an integer representing the major version
execute_process(
  COMMAND ${PARSE_COMMAND} -m
  WORKING_DIRECTORY ${SCRIPTS_DIR}
  OUTPUT_VARIABLE VERSION_MAJOR
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Stores an integer representing the minor version
execute_process(
  COMMAND ${PARSE_COMMAND} -n
  WORKING_DIRECTORY ${SCRIPTS_DIR}
  OUTPUT_VARIABLE VERSION_MINOR
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Stores an integer representing the patch version
execute_process(
  COMMAND ${PARSE_COMMAND} -p
  WORKING_DIRECTORY ${SCRIPTS_DIR}
  OUTPUT_VARIABLE VERSION_PATCH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Stores the 8 digit short SHA of the current commit
execute_process(
  COMMAND ${PARSE_COMMAND} -c
  WORKING_DIRECTORY ${SCRIPTS_DIR}
  OUTPUT_VARIABLE VERSION_COMMIT
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Stores "dirty" or "" if built off a commit that hasn't been
# pushed or if build from a modified working directory
execute_process(
  COMMAND ${PARSE_COMMAND} -D
  WORKING_DIRECTORY ${SCRIPTS_DIR}
  OUTPUT_VARIABLE VERSION_DIRTY
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Stores "develop" or "" if built off a commit without a tag on it
execute_process(
  COMMAND ${PARSE_COMMAND} -d
  WORKING_DIRECTORY ${SCRIPTS_DIR}
  OUTPUT_VARIABLE VERSION_DEVELOP
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Stores a full version string in the form of:
#   {MAJOR}.{MINOR}.{PATCH}[-develop][-dirty]+{COMMIT}
# Example:
#   8.3.1-develop+db9d63ec
execute_process(
  COMMAND ${PARSE_COMMAND} -f
  WORKING_DIRECTORY ${SCRIPTS_DIR}
  OUTPUT_VARIABLE VERSION_FULL
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Stores the tag of the most recent version. Example:
#   v8.3.1
execute_process(
  COMMAND ${PARSE_COMMAND} -t
  WORKING_DIRECTORY ${SCRIPTS_DIR}
  OUTPUT_VARIABLE VERSION_TAG
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

message("Version: ${VERSION_FULL}")

add_definitions("-DVERSION_MAJOR=\"${VERSION_MAJOR}\"")
add_definitions("-DVERSION_MINOR=\"${VERSION_MINOR}\"")
add_definitions("-DVERSION_PATCH=\"${VERSION_PATCH}\"")
add_definitions("-DVERSION_COMMIT=\"${VERSION_COMMIT}\"")
add_definitions("-DVERSION_DIRTY=\"${VERSION_DIRTY}\"")
add_definitions("-DVERSION_DEVELOP=\"${VERSION_DEVELOP}\"")
add_definitions("-DVERSION_TAG=\"${VERSION_TAG}\"")
add_definitions("-DVERSION_FULL=\"${VERSION_FULL}\"")
