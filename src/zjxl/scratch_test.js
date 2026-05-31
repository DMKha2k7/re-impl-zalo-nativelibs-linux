const fs = require('fs');
const content = fs.readFileSync('/mnt/SSD/zalo-for-linux-project/nativelibs/zjxl/build/linux_x64/jxl.node', 'utf8');
console.log('Has resizeJxlLimit:', content.includes('resizeJxlLimit'));
console.log('Has resizeJxl:', content.includes('resizeJxl'));
console.log('Has jxlToJpeg:', content.includes('jxlToJpeg'));
