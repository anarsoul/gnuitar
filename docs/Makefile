source=readme.html
target=index.html

all: $(target)

$(target): $(source)
	echo "<html><head>" \
	"<title>GNUitar Manual - Table of Contents</title>" \
	"</head>" \
	"<body>" \
	"<h1>GNUitar Manual - Table of Contents</h1>" \
	"<ul>" > $(target)
	cat $(source) | grep -i '<h.>.*</h.>' | sed "s/a name=/a href=$(source)#/" | \
	    tr -d "\n" | \
	    sed "s/<\/h1><h2>/<ul><li>/g" | \
	    sed "s/<\/h2><h3>/<ul><li>/g" | \
	    sed "s/<\/h3><h2>/<\/li><\/ul><li>/g" | \
	    sed "s/<\/h2><h1>/<\/li><\/ul><li>/g" | \
	    sed "s/<\/h.>/<\/li>/g" | \
	    sed "s/<h.>/<li>/g" | \
	    sed "s/<\/li>/<\/li>%/g" | \
	    tr % "\n" \
	>> $(target)
	echo "</ul></body></html>" >> $(target)

$(source):
