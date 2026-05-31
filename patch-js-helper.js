/**
 * patch-js-helper.js
 * Applies Linux-specific modifications to original macOS JS wrappers in memory.
 */

'use strict';

const fs = require('fs');
const path = require('path');

const MACOS_DIR = path.resolve(__dirname, './nativelibs-macOS');

const patches = {
  'db-cross-v4/dist/binding.js': {
    search: `else {\n    if (process.arch === 'x64') {`,
    replace: `else if (process.platform === 'linux') {\n    if (process.arch === 'x64') {\n        addon = require('../prebuilt/linux/electron_x86_64/db-cross-v4-native.node');\n    }\n    else {\n        addon = require('../prebuilt/linux/electron_x86/db-cross-v4-native.node');\n    }\n}\nelse {\n    if (process.arch === 'x64') {`
  },
  'file-utilities/index.js': {
    search: `    default:\n      throw new Error(\`Unsupported OS: \${platform}, architecture: \${arch}\`)\n  }\n}`,
    replace: `    case 'linux':\n      switch (arch) {\n        case 'x64':\n          return join(__dirname, 'linux', 'file-utilities.node')\n        default:\n          throw new Error(\`Unsupported architecture on Linux: \${arch}\`)\n      }\n    default:\n      throw new Error(\`Unsupported OS: \${platform}, architecture: \${arch}\`)\n  }\n}`
  },
  'file-utils/index.js': {
    search: `  } else {\n    return {error: 'not support'};\n  }`,
    replace: `  } else if (process.platform === 'linux'){\n    if (process.arch === 'x64') {\n      return require('./linux/file-utils.node');\n    }\n    return {error: 'linux architecture not supported'};\n  } else {\n    return {error: 'not support'};\n  }`
  },
  'mp4thumb/index.js': {
    search: `        else {\n            if(process.arch === 'arm64'){`,
    replace: `        else if(process.platform === 'linux') {\n            thumbModule = require(\`./linux-\${process.arch}/mp4thumb.node\`);\n        }\n        else {\n            if(process.arch === 'arm64'){`
  },
  'zimage/index.js': {
    search: `\t\t} else {\n\t\t\tos = 'darwin_x64';\n\t\t}\n\t}\n}`,
    replace: `\t\t} else {\n\t\t\tos = 'darwin_x64';\n\t\t}\n\t} else if (process.platform === 'linux') {\n\t\tos = 'linux_x64';\n\t}\n}`
  },
  'zjxl/index.js': {
    search: `  } else {\n    return { error: 'not support' };\n  }`,
    replace: `  } else if (process.platform === 'linux') {\n    if (process.arch === 'arm64') nodeAddon = require('./build/linux_arm64/jxl.node');\n    else nodeAddon = require('./build/linux_x64/jxl.node');\n  } else {\n    return { error: 'not support' };\n  }`
  }
};

function patchFile(relativePath, destPath) {
  const srcPath = path.join(MACOS_DIR, relativePath);
  if (!fs.existsSync(srcPath)) {
    throw new Error(`Original macOS file not found: ${srcPath}`);
  }

  let content = fs.readFileSync(srcPath, 'utf8');
  const patch = patches[relativePath];

  if (patch) {
    // Normalize newlines to \n to make sure replacements work consistently across environments
    const normalizedContent = content.replace(/\r\n/g, '\n');
    const normalizedSearch = patch.search.replace(/\r\n/g, '\n');
    const normalizedReplace = patch.replace.replace(/\r\n/g, '\n');

    if (!normalizedContent.includes(normalizedSearch)) {
      throw new Error(`Failed to apply patch for ${relativePath}: Search target pattern not found.`);
    }
    content = normalizedContent.replace(normalizedSearch, normalizedReplace);
    console.log(`✨ Applied JS Patch to: ${relativePath}`);
  } else {
    console.log(`📄 Copying unchanged JS wrapper: ${relativePath}`);
  }

  // Prepend library prefix to all console outputs and error objects/throws
  const libName = relativePath.split('/')[0];
  content = content.replace(/console\.(log|error|warn|info|debug|dir|trace)\s*\(/g, `console.$1('[${libName}]', `);
  content = content.replace(/new\s+Error\(/g, `new Error('[${libName}] ' + `);
  content = content.replace(/error:\s*['"]not support['"]/g, `error: '[${libName}] not support'`);
  content = content.replace(/message:\s*['"]Failed to load mp4thumb module['"]/g, `message: '[mp4thumb] Failed to load mp4thumb module'`);
  content = content.replace(/message:\s*['"]linux architecture not supported['"]/g, `message: '[${libName}] linux architecture not supported'`);

  const parent = path.dirname(destPath);
  if (!fs.existsSync(parent)) {
    fs.mkdirSync(parent, { recursive: true });
  }
  fs.writeFileSync(destPath, content, 'utf8');
}

module.exports = { patchFile };
