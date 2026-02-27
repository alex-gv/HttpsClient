#include <private/ssl_context_builder.h>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <memory>
#include <vector>

namespace https_client {
class SSLCustomContextBuilder::Impl {
 public:
    std::unique_ptr<boost::asio::ssl::context> CreateContext(boost::asio::ssl::context_base::method method) {
        return std::make_unique<boost::asio::ssl::context>(method);
    }

 private:
    void AddLinuxSystemCertificates(boost::asio::ssl::context& ctx) {
        SSL_CTX* ssl_ctx = ctx.native_handle();
        X509_STORE* store = SSL_CTX_get_cert_store(ssl_ctx);
        if (!store) {
            throw std::runtime_error("Cannot get X509 store from SSL context");
        }

        const char* cert_paths[] = {"/etc/ssl/certs/ca-certificates.crt",  // Debian/Ubuntu
                                    "/etc/pki/tls/certs/ca-bundle.crt",    // Fedora/RHEL
                                    "/etc/ssl/ca-bundle.pem",              // OpenSUSE
                                    "/etc/pki/tls/cacert.pem",             // Older systems
                                    "/etc/ssl/cert.pem",                   // Alpine Linux
                                    nullptr};

        bool loaded = false;
        for (const char** path = cert_paths; *path != nullptr; ++path) {
            boost::system::error_code ec;
            ctx.load_verify_file(*path, ec);
            loaded = true;
            break;
        }

        if (!loaded) {
            const char* cert_dirs[] = {"/etc/ssl/certs",      // Debian/Ubuntu
                                       "/etc/pki/tls/certs",  // Fedora/RHEL
                                       "/etc/ssl",            // OpenSUSE
                                       nullptr};

            for (const char** dir = cert_dirs; *dir != nullptr; ++dir) {
                boost::system::error_code ec;
                ctx.add_verify_path(*dir, ec);
                loaded = true;
            }
        }
        boost::system::error_code ec;
        ctx.set_default_verify_paths(ec);
    }
};

std::unique_ptr<boost::asio::ssl::context> SSLCustomContextBuilder::CreateContext(
    boost::asio::ssl::context_base::method method) {
    return impl_->CreateContext(method);
};

}  // namespace https_client