export const PICTREE_DATA = [
	{
		name: 'Beach',
		image: 'images/beach.jpg',
		annotation: 'A beach.'
	},
	{
		name: 'Mountain',
		image: 'images/mountain-wide.jpg',
		annotation: 'Mountain wide landscape.'
	},
	{
		name: 'Castle',
		image: 'images/castle-high.jpg',
		annotation: 'Castle high portrait.'
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

