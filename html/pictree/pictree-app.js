import Split from './split.js';

const DEMO_TREE = [
	{
		name: 'Beach',
		image: 'images/beach.jpg',
		annotation: 'A beach.'
	},
	{
		name: 'Mountain',
		image: 'images/mountain.jpg',
		annotation: 'Mountain hiking.'
	},
	{
		name: 'Animals',
		children: [
			{
				name: 'Cat',
				image: 'images/cat.jpg',
				annotation: 'A cat.'
			},
			{
			name: 'Dog',
				image: 'images/dog.jpg',
				annotation: 'A dog.'
			}
		]
	},
];


function initTree() {
  const container = document.getElementById('tree-pane');

  const tree = new TreeView(DEMO_TREE, container);

  tree.on('select', evt => {
    // evt = { target, data }
    const item = evt.data;

    if (item) {
      showNode(item);
    }
  });
}


function showNode(node) {
  document.getElementById('image-viewer').src = node.image;
  document.getElementById('annotation').textContent = node.annotation;
}

function initLayout() {
  Split(['#tree-pane', '#viewer-pane'], {
    sizes: [30, 70],
    minSize: 150,
    gutterSize: 6
  });
}

function initViewer() {
  const img = document.getElementById('image-viewer');
  const ann = document.getElementById('annotation');

  img.src = '';
  ann.textContent = 'Select a node from the tree.';
}


initLayout();
initTree();

initViewer();
