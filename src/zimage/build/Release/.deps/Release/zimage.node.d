cmd_Release/zimage.node := ln -f "Release/obj.target/zimage.node" "Release/zimage.node" 2>/dev/null || (rm -rf "Release/zimage.node" && cp -af "Release/obj.target/zimage.node" "Release/zimage.node")
