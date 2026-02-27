#include <private/ssl_context_builder.h>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <memory>
#include <vector>
#include <stdexcept>

#include <windows.h>
#include <wincrypt.h>
#pragma comment(lib, "crypt32.lib")

namespace https_client {
class SSLCustomContextBuilder::Impl {
 public:
    std::unique_ptr<boost::asio::ssl::context> CreateContext(boost::asio::ssl::context_base::method method) {
        auto ctx = std::make_unique<boost::asio::ssl::context>(method);
        AddWindowsSystemCertificates(*ctx);
        return ctx;
    }

 private:
    void AddWindowsSystemCertificates(boost::asio::ssl::context& ctx) {
        SSL_CTX* ssl_ctx = ctx.native_handle();
        X509_STORE* store = SSL_CTX_get_cert_store(ssl_ctx);

        if (!store) {
            throw std::runtime_error("Cannot get X509 store from SSL context");
        }

        AddCertificatesFromStore(store, CERT_SYSTEM_STORE_CURRENT_USER, L"ROOT");
        AddCertificatesFromStore(store, CERT_SYSTEM_STORE_CURRENT_USER, L"CA");
        AddCertificatesFromStore(store, CERT_SYSTEM_STORE_LOCAL_MACHINE, L"ROOT");
        AddCertificatesFromStore(store, CERT_SYSTEM_STORE_LOCAL_MACHINE, L"CA");

        AddCertificatesFromStore(store, CERT_SYSTEM_STORE_CURRENT_USER, L"INTERMEDIATE");
        AddCertificatesFromStore(store, CERT_SYSTEM_STORE_LOCAL_MACHINE, L"INTERMEDIATE");

        AddCertificatesFromStore(store, CERT_SYSTEM_STORE_CURRENT_USER, L"TRUSTEDPUBLISHER");
        AddCertificatesFromStore(store, CERT_SYSTEM_STORE_LOCAL_MACHINE, L"TRUSTEDPUBLISHER");

        boost::system::error_code ec;
        ctx.set_default_verify_paths(ec);
    }

    void AddCertificatesFromStore(X509_STORE* store, DWORD system_store_location, LPCWSTR store_name) {
        HCERTSTORE hStore = CertOpenStore(CERT_STORE_PROV_SYSTEM, 0, NULL,
                                          system_store_location | CERT_STORE_READONLY_FLAG, store_name);

        if (!hStore) {
            return;
        }

        PCCERT_CONTEXT pCertContext = nullptr;
        while ((pCertContext = CertEnumCertificatesInStore(hStore, pCertContext)) != nullptr) {
            const unsigned char* p = pCertContext->pbCertEncoded;
            X509* x509 = d2i_X509(nullptr, &p, pCertContext->cbCertEncoded);

            if (x509) {
                X509_STORE_add_cert(store, x509);
                X509_free(x509);
            }
        }
        CertCloseStore(hStore, 0);
    }

    void AddCertificatesFromChain(X509_STORE* store, PCCERT_CONTEXT pCertContext) {
        PCCERT_CHAIN_CONTEXT pChainContext = nullptr;
        CERT_CHAIN_PARA chainPara = {sizeof(chainPara)};

        if (CertGetCertificateChain(nullptr, pCertContext, nullptr, nullptr, &chainPara, 0, nullptr, &pChainContext)) {
            for (DWORD i = 0; i < pChainContext->cChain; i++) {
                PCERT_SIMPLE_CHAIN pSimpleChain = pChainContext->rgpChain[i];
                for (DWORD j = 0; j < pSimpleChain->cElement; j++) {
                    PCERT_CHAIN_ELEMENT pElement = pSimpleChain->rgpElement[j];

                    const unsigned char* p = pElement->pCertContext->pbCertEncoded;
                    X509* x509 = d2i_X509(nullptr, &p, pElement->pCertContext->cbCertEncoded);

                    if (x509) {
                        X509_STORE_add_cert(store, x509);
                        X509_free(x509);
                    }
                }
            }
            CertFreeCertificateChain(pChainContext);
        }
    }
};

std::unique_ptr<boost::asio::ssl::context> SSLCustomContextBuilder::CreateContext(
    boost::asio::ssl::context_base::method method) {
    return impl_->CreateContext(method);
};

}  // namespace https_client