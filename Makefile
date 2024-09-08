CC = gcc
CFLAGS = -o
TARGET = webSocketServer.exe
SRC = webSocketServer.c

HTML_DIR = html
GZIP_DIR = gzipHtml
INDEX_FILE = index.html
STYLES_DIR = styles

all: $(TARGET) gzip_files

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(TARGET) $(SRC)


gzip_files: 
	gzip -k -r $(HTML_DIR) && mkdir -p $(GZIP_DIR) && mv $(HTML_DIR)/*.gz $(GZIP_DIR)
	gzip -k $(INDEX_FILE)
	gzip -k -r $(STYLES_DIR)

clean:
	rm -f $(TARGET)
	rm -f $(HTML_DIR)/*.gz
	rm -f $(INDEX_FILE).gz
	rm -f $(STYLES_DIR)/*.gz
	rm -r -f $(GZIP_DIR)

start:
	sudo ./webSocketServer.exe 8000