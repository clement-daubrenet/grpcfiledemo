from threading import Thread
import subprocess


def server():
    subprocess.run(['../server'], stdout=subprocess.PIPE)


if __name__ == '__main__':
    server = Thread(target=server)
    server.start()
    client = subprocess.Popen("../client", stdout=subprocess.PIPE, shell=True)
    out, err = client.communicate()

    # Checking that the client and the server return the right signals (parameters and file sent and received)
    assert out == b'[CLIENT] Sending the string and the number to the server ... \n' \
                  b'[SERVER] ...Received a string: test-string and a number: 17\n' \
                  b'[CLIENT] Sending the file by chunk to the server... \n' \
                  b'[SERVER] ...Finished to host the file\n'
    print('Test OK')
    server.join()
    client.kill()
