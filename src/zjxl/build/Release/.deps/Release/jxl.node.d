cmd_Release/jxl.node := ln -f "Release/obj.target/jxl.node" "Release/jxl.node" 2>/dev/null || (rm -rf "Release/jxl.node" && cp -af "Release/obj.target/jxl.node" "Release/jxl.node")
