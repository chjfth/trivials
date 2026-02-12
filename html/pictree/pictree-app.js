import Split from './split.js';

function getImageBase() {
  const params = new URLSearchParams(window.location.search);
  const dir = params.get('imgdir');
  return dir ? dir.replace(/^\/+|\/+$/g, '') + '/' : '';
}

const IMAGE_BASE = getImageBase();


async function loadUserData() {
  const modulePath = IMAGE_BASE + 'pictree-userdata.js';

  try {
    const mod = await import(`./${modulePath}`);
    return mod.PICTREE_DATA;
  } catch (err) {
    console.error('Failed to load user data:', modulePath, err);
    alert('Cannot load pictree-userdata.js from ' + modulePath);
    throw err;
  }
}



function initTree(treeData) {

  // validateTree(PICTREE_DATA); xxx

  const container = document.getElementById('tree-pane');
  const tree = new TreeView(treeData, container);
  
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
  const first = findFirstImageNode(treeData);
  if (first) {
    showNode(first);
    // tree.select(first);   // keeps tree selection in sync
  }  
}


function setAnnotationHTML(el, html) {
  // Very lightweight sanitation, remove <script> from HTML raw text.
  el.innerHTML = html.replace(/<script[\s\S]*?>[\s\S]*?<\/script>/gi, '');
}


function showNode(node) {
  const img = document.getElementById('image-viewer');
  img.onload = fitImageWithAnnotation;
  
  const ann = document.getElementById('annotation');
  
  if (!node.image)
  	return;
  
  img.src = IMAGE_BASE + node.image;
  img.title = img.src;
  setAnnotationHTML(ann, node.annotation || '');
  
  // Let layout settle, then measure
  requestAnimationFrame(fitImageWithAnnotation);
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


function fitImageWithAnnotation() {
  const pane = document.getElementById('viewer-pane');
  const img  = document.getElementById('image-viewer');
  const ann  = document.getElementById('annotation');

  if (!img.src) return;

  const paneHeight = pane.clientHeight;
  const annHeight  = ann.scrollHeight;

  // Annotation capped to half viewport
  const maxAnnHeight = paneHeight / 2;
  const effectiveAnnHeight = Math.min(annHeight, maxAnnHeight);

  const availableForImage = paneHeight - effectiveAnnHeight - 8;
  img.style.maxHeight = `${availableForImage}px`;

  // Decide whether to center or top-align
  const imgHeight = img.getBoundingClientRect().height;
  const totalContentHeight = imgHeight + annHeight + 8;

  if (totalContentHeight < paneHeight) {
    pane.classList.add('centered');
  } else {
    pane.classList.remove('centered');
  }
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

  window.addEventListener('resize', fitImageWithAnnotation);
}


async function main() {
  initLayout();
  initViewer();

  const treeData = await loadUserData();
  initTree(treeData);
}

main();


