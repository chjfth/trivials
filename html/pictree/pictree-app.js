import Split from './split.js';

import { PICTREE_DATA } from './pictree-userdata.js';

const IMAGE_BASE = getImageBasePath();


function initTree() {

  validateTree(PICTREE_DATA);

  const container = document.getElementById('tree-pane');
  const tree = new TreeView(PICTREE_DATA, container);
  
  tree.on('select', evt => {
    // evt = { target, data }
    const item = evt.data;

	// Remove previous selection
	document
		.querySelectorAll('.tree-leaf-content.selected')
		.forEach(el => el.classList.remove('selected'));

	// Highlight newly selected node
	const content = evt.target.target.closest('.tree-leaf-content');
	if (content) {
		content.classList.add('selected');
	}

    if (item) {
      showNode(item);
    }
  });
  
  // Auto-show first image node on load
  const first = findFirstImageNode(PICTREE_DATA);
  if (first) {
    showNode(first);
    // tree.select(first);   // keeps tree selection in sync
  }  
}


function showNode(node) {
  const img = document.getElementById('image-viewer');
  const ann = document.getElementById('annotation');
  
  if (!node.image)
  	return;
  
  img.src = IMAGE_BASE + node.image;
  img.title = img.src;
  ann.textContent = node.annotation || '';

}

function validateTree(nodes, path = 'root') {
  if (!Array.isArray(nodes)) {
    throw new Error(`Tree data at ${path} must be an array`);
  }

  nodes.forEach((node, index) => {
    const here = `${path}[${index}]`;

    if (!node.name) {
      throw new Error(`Missing "name" at ${here}`);
    }

    const hasImage = 'image' in node;
    const hasChildren = Array.isArray(node.children);

    if (hasChildren) {
      validateTree(node.children, `${here}.children`);
    } else {
      if (!hasImage) {
        throw new Error(
          `Leaf node "${node.name}" at ${here} must have an "image"`
        );
      }
    }
  });
}


function findFirstImageNode(nodes) {
  for (const node of nodes) {
    if (node.image) {
      return node;
    }
    if (node.children) {
      const found = findFirstImageNode(node.children);
      if (found) return found;
    }
  }
  return null;
}


function getImageBasePath() {
  const params = new URLSearchParams(window.location.search);
  const dir = params.get('imgdir');

  // No imgdir → current directory
  if (!dir) return '';

  // Normalize: remove leading/trailing slashes
  return dir.replace(/^\/+|\/+$/g, '') + '/';
}


function initLayout() {
  Split(['#tree-pane', '#viewer-pane'], {
    sizes: [20, 80],
    minSize: 150,
    gutterSize: 6
  });
}

function initViewer() {
  const img = document.getElementById('image-viewer');
  const ann = document.getElementById('annotation');

  img.src = '';
  ann.textContent = 'Select a node from the tree.';

  img.onerror = () => {
    ann.textContent = 'Image not found: ' + img.src;
  };
}




initLayout();
initViewer();
initTree();

