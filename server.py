import subprocess
from http.server import BaseHTTPRequestHandler, HTTPServer
import json
from PIL import Image

class RequestHandler(BaseHTTPRequestHandler):
    def _set_response(self, content_type, render_time):
        self.send_response(200)
        self.send_header('Content-type', content_type)
        self.send_header('Render-time', render_time)
        self.end_headers()

    def do_POST(self):
        content_length = int(self.headers['Content-Length'])
        post_data = self.rfile.read(content_length)
        dict = json.loads(post_data)

        args = ['build\\ray-tracer.exe',
        str(dict['num_threads']), # Numero de hilos : entero : 1
        str(dict['num_proporcion']), # Numerador de la proporcion : entero : 2
        str(dict['den_proporcion']), # Denominador de la proporcion : entero : 3
        str(dict['ancho_img']), # Ancho de la imagen : entero : 4
        str(dict['muestras_pixel']), # Muestras por pixel : entero : 5
        str(dict['max_rayos']), # Numero maximo de rayos : entero : 6
        str(dict['x_coord']), # Coordenada X de la camara : double : 7
        str(dict['y_coord']), # Coordenada Y de la camara : double : 8
        str(dict['z_coord']), # Coordenada Z de la camara : double : 9
        str(dict['R_e1']), # Valor R para esfera 1 : entero : 10
        str(dict['G_e1']), # Valor G para esfera 1 : entero : 11
        str(dict['B_e1']), # Valor B para esfera 1 : entero : 12
        str(dict['R_e2']), # Valor R para esfera 2 : entero : 13
        str(dict['G_e2']), # Valor G para esfera 2 : entero : 14
        str(dict['B_e2']), # Valor B para esfera 2 : entero : 15
        str(dict['ref']), # Indice de refraccion : double : 16
        str(dict['rand']), # Esferas aleatorias : double : 17
		]

        result = subprocess.run(args, capture_output=True, shell=True)
        stdout_str = result.stdout.decode('utf-8')

        im = Image.open("image.ppm")
        im.save("image.jpg")

        self._set_response('image/jpg', stdout_str)

        with open('image.jpg', 'rb') as file:
            self.wfile.write(file.read())


def run_server(port=5050):
    server_address = ('', port)
    httpd = HTTPServer(server_address, RequestHandler)
    print(f'Starting server on port {port}')
    httpd.serve_forever()


if __name__ == '__main__':
    run_server()
