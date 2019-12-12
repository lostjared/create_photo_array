# create_photo_array

This program creates some pages for my website to display my artwork.
Thought I would upload it here for anyone who is interested how it was done.

To use this program use the following arguments
	-p path
	-n no output thumbnail
	-r prefix string
	-t thumbnail prefix
	-s page title

to generate thumbnails for my site use:

	$ create_photo_array -p .

to generate for a directory on your site use:

	$ create_photo_array -p path -r https://site/dir/ -t https://site/dir/thumbnail

or to use current path

	$ create_photo_array -p path -r "" -t "" -s "Page title"

or desired path like this

	$ create_photo_array -p . -r http://lostsidedead.biz/folder/on/my/site/ -t https://lostsidedead.biz/folder/on/my/site/thumbnaildir

and finally to generate just html pages use

	$ create_photo_array -p . -n

