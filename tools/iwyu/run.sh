#!/bin/zsh

module=inviwo-module-opengl

sed=gsed # osx sed version is odd
fix=/opt/homebrew/Cellar/include-what-you-use/0.16_1/bin/fix_includes.py

ninja -t clean ${module}

ninja ${module} | tee inc.fix 

${sed} -i -E 's/#include "([a-z./]+)"/#include <\1>/g' inc.fix
${sed} -i 's/#include <stdint.h>/#include <cstdint> /g' inc.fix
${sed} -i 's/#include <stddef.h>/#include <cstddef> /g' inc.fix
${sed} -i 's/#include <math.h>/#include <cmath> /g' inc.fix
${sed} -i 's/#include <string.h>/#include <cstring> /g' inc.fix

#${fix} --only_re=".*/opengl/.*" --comments --blank_lines --safe_headers --separate_project_includes="<tld>" < inc.fix
${fix} --only_re=".*/opengl/.*" --comments --blank_lines --nosafe_headers --separate_project_includes="<tld>" < inc.fix


