/*
  Copyright (c) 2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warrenty.
*/


#ifndef GLOOX_H__
#define GLOOX_H__

#include <string>
#include <list>
#include <map>

#define XMLNS_CLIENT            "jabber:client"
#define XMLNS_COMPONENT_ACCEPT  "jabber:component:accept"
#define XMLNS_COMPONENT_CONNECT "jabber:component:connect"

#define XMLNS_DISCO_INFO        "http://jabber.org/protocol/disco#info"
#define XMLNS_DISCO_ITEMS       "http://jabber.org/protocol/disco#items"
#define XMLNS_ADHOC_COMMANDS    "http://jabber.org/protocol/commands"
#define XMLNS_ROSTER            "jabber:iq:roster"
#define XMLNS_VERSION           "jabber:iq:version"
#define XMLNS_REGISTER          "jabber:iq:register"
#define XMLNS_PRIVACY           "jabber:iq:privacy"
#define XMLNS_AUTH              "jabber:iq:auth"
#define XMLNS_PRIVATE_XML       "jabber:iq:private"
#define XMLNS_ROSTER            "jabber:iq:roster"
#define XMLNS_BOOKMARKS         "storage:bookmarks"
#define XMLNS_ANNOTATIONS       "storage:rosternotes"

#define XMLNS_XMPP_STANZAS      "urn:ietf:params:xml:ns:xmpp-stanzas"
#define XMLNS_STREAM_TLS        "urn:ietf:params:xml:ns:xmpp-tls"
#define XMLNS_STREAM_SASL       "urn:ietf:params:xml:ns:xmpp-sasl"
#define XMLNS_STREAM_BIND       "urn:ietf:params:xml:ns:xmpp-bind"
#define XMLNS_STREAM_SESSION    "urn:ietf:params:xml:ns:xmpp-session"
#define XMLNS_STREAM_IQAUTH     "http://jabber.org/features/iq-auth"
#define XMLNS_STREAM_IQREGISTER "http://jabber.org/features/iq-register"

#define XMPP_STREAM_VERSION_MAJOR  "1"
#define XMPP_STREAM_VERSION_MINOR  "0"
#define GLOOX_VERSION              "0.5"

/**
 * The namespace for the gloox library.
 * @author Jakob Schroeter <js@camaya.net>
 * @since 0.3
 */
namespace gloox
{

  /**
   * This describes the possible states of a stream.
   */
  enum ConnectionState
  {
    STATE_DISCONNECTED,             /**< The client is in disconnected state. */
    STATE_CONNECTING,               /**< The client is currently trying to establish a connection. */
    STATE_CONNECTED,                /**< The client is connected to the server but authentication is not
                                     * (yet) done. */
  };

  /**
   * This describes connection error conditions.
   */
  enum ConnectionError
  {
    CONN_OK,                       /**< Not really an error. Everything went just fine. */
    CONN_STREAM_ERROR,             /**< An stream error occured. The stream has been closed. */
    CONN_STREAM_CLOSED,            /**< The stream has been closed graciously. */
    CONN_IO_ERROR,                 /**< An I/O error occured. */
    CONN_OUT_OF_MEMORY,            /**< Out of memory. Uhoh. */
    CONN_NO_SUPPORTED_AUTH,        /**< The auth mechanisms the server offers are not supported. */
    CONN_TLS_FAILED,               /**< The server's certificate could not be verified. */
    CONN_AUTHENTICATION_FAILED,    /**< Authentication failed. Username/password wrong or account does
                                     * not exist. */
    CONN_USER_DISCONNECTED,        /**< The user (or higher-level protocol) requested a disconnecct. */
  };

  /**
   * This decribes stream error conditions as defined in RFC 3920 Sec. 4.7.3.
   */
  enum StreamError
  {
    ERROR_UNDEFINED,                /**< An undefined/unknown error occured. Also used if a diconnect was
                                     * user-initiated. Also set before and during a established connection
                                    (where obviously no error occured). */
    ERROR_BAD_FORMAT,               /**< The entity has sent XML that cannot be processed;
                                     * this error MAY be used instead of the more specific XML-related
                                     * errors, such as &lt;bad-namespace-prefix/&gt;, &lt;invalid-xml/&gt;,
                                     * &lt;restricted-xml/&gt;, &lt;unsupported-encoding/&gt;, and
                                     * &lt;xml-not-well-formed/&gt;, although the more specific errors are
                                     * preferred. */
    ERROR_BAD_NAMESPACE_PREFIX,     /**< The entity has sent a namespace prefix that is unsupported, or has
                                     * sent no namespace prefix on an element that requires such a prefix
                                     * (see XML Namespace Names and Prefixes (Section 11.2)). */
    ERROR_CONFLICT,                 /**< The server is closing the active stream for this entity because a
                                     * new stream has been initiated that conflicts with the existing
                                     * stream. */
    ERROR_CONNECTION_TIMEOUT,       /**< The entity has not generated any traffic over the stream for some
                                     * period of time (configurable according to a local service policy).*/
    ERROR_HOST_GONE,                /**< the value of the 'to' attribute provided by the initiating entity
                                     * in the stream header corresponds to a hostname that is no longer
                                     * hosted by the server.*/
    ERROR_HOST_UNKNOWN,             /**< The value of the 'to' attribute provided by the initiating entity
                                     * in the stream header does not correspond to a hostname that is hosted
                                     * by the server. */
    ERROR_IMPROPER_ADDRESSING,      /**< A stanza sent between two servers lacks a 'to' or 'from' attribute
                                     * (or the attribute has no value). */
    ERROR_INTERNAL_SERVER_ERROR,    /**< the server has experienced a misconfiguration or an
                                     * otherwise-undefined internal error that prevents it from servicing the
                                     * stream. */
    ERROR_INVALID_FROM,             /**< The JID or hostname provided in a 'from' address does not match an
                                     * authorized JID or validated domain negotiated between servers via SASL
                                     * or dialback, or between a client and a server via authentication and
                                     * resource binding.*/
    ERROR_INVALID_ID,               /**< The stream ID or dialback ID is invalid or does not match an ID
                                     * previously provided. */
    ERROR_INVALID_NAMESPACE,        /**< The streams namespace name is something other than
                                     * "http://etherx.jabber.org/streams" or the dialback namespace name is
                                     * something other than "jabber:server:dialback" (see XML Namespace Names
                                     * and Prefixes (Section 11.2)). */
    ERROR_INVALID_XML,              /**< The entity has sent invalid XML over the stream to a server that
                                     * performs validation (see Validation (Section 11.3)). */
    ERROR_NOT_AUTHORIZED,           /**< The entity has attempted to send data before the stream has been
                                     * authenticated, or otherwise is not authorized to perform an action
                                     * related to stream negotiation; the receiving entity MUST NOT process
                                     * the offending stanza before sending the stream error. */
    ERROR_POLICY_VIOLATION,         /**< The entity has violated some local service policy; the server MAY
                                     * choose to specify the policy in the &lt;text/&gt;  element or an
                                     * application-specific condition element. */
    ERROR_REMOTE_CONNECTION_FAILED, /**< The server is unable to properly connect to a remote entity that is
                                     * required for authentication or authorization. */
    ERROR_RESOURCE_CONSTRAINT,      /**< the server lacks the system resources necessary to service the
                                     * stream. */
    ERROR_RESTRICTED_XML,           /**< The entity has attempted to send restricted XML features such as a
                                     * comment, processing instruction, DTD, entity reference, or unescaped
                                     * character (see Restrictions (Section 11.1)). */
    ERROR_SEE_OTHER_HOST,           /**< The server will not provide service to the initiating entity but is
                                     * redirecting traffic to another host; the server SHOULD specify the
                                     * alternate hostname or IP address (which MUST be a valid domain
                                     * identifier) as the XML character data of the &lt;see-other-host/&gt;
                                     * element. */
    ERROR_SYSTEM_SHUTDOWN,          /**< The server is being shut down and all active streams are being
                                     * closed. */
    ERROR_UNDEFINED_CONDITION,      /**< The error condition is not one of those defined by the other
                                     * conditions in this list; this error condition SHOULD be used only in
                                     * conjunction with an application-specific condition. */
    ERROR_UNSUPPORTED_ENCODING,     /**< The initiating entity has encoded the stream in an encoding that is
                                     * not supported by the server (see Character Encoding (Section 11.5)). */
    ERROR_UNSUPPORTED_STANZA_TYPE,  /**< The initiating entity has sent a first-level child of the stream
                                     * that is not supported by the server. */
    ERROR_UNSUPPORTED_VERSION,      /**< The value of the 'version' attribute provided by the initiating
                                     * entity in the stream header specifies a version of XMPP that is not
                                     * supported by the server; the server MAY specify the version(s) it
                                     * supports in the &lt;text/&gt; element. */
    ERROR_XML_NOT_WELL_FORMED,      /**< The initiating entity has sent XML that is not well-formed as
                                     * defined by [XML]. */
  };

  /**
   * Describes the possible stanza types.
   */
  enum StanzaType
  {
    STANZA_UNDEFINED,               /**< Undefined. */
    STANZA_IQ,                      /**< An Info/Query stanza. */
    STANZA_MESSAGE,                 /**< A message stanza. */
    STANZA_S10N,                    /**< A presence/subscription stanza. */
    STANZA_PRESENCE,                /**< A presence stanza. */
  };

  /**
   * Describes the possible stanza-sub-types.
   */
  enum StanzaSubType
  {
    STANZA_SUB_UNDEFINED,           /**< Undefined. */
    STANZA_IQ_GET,                  /**< The stanza is a request for information or requirements. */
    STANZA_IQ_SET,                  /**<
                                     * The stanza provides required data, sets new values, or
                                     * replaces existing values.
                                     */
    STANZA_IQ_RESULT,               /**< The stanza is a response to a successful get or set request. */
    STANZA_IQ_ERROR,                /**<
                                     * An error has occurred regarding processing or
                                     * delivery of a previously-sent get or set (see Stanza Errors
                                     * (Section 9.3)).
                                     */
    STANZA_PRES_UNAVAILABLE,        /**<
                                     * Signals that the entity is no longer available for
                                     * communication.
                                     */
    STANZA_PRES_AVAILABLE,          /**<
                                     * Signals to the server that the sender is online and available
                                     * for communication.
                                     */
    STANZA_PRES_PROBE,              /**<
                                     * A request for an entity's current presence; SHOULD be
                                     * generated only by a server on behalf of a user.
                                     */
    STANZA_PRES_ERROR,              /**<
                                     * An error has occurred regarding processing or delivery of
                                     * a previously-sent presence stanza.
                                     */
    STANZA_S10N_SUBSCRIBE,          /**<
                                     * The sender wishes to subscribe to the recipient's
                                     * presence.
                                     */
    STANZA_S10N_SUBSCRIBED,         /**<
                                     * The sender has allowed the recipient to receive
                                     * their presence.
                                     */
    STANZA_S10N_UNSUBSCRIBE,        /**<
                                     * The sender is unsubscribing from another entity's
                                     * presence.
                                     */
    STANZA_S10N_UNSUBSCRIBED,       /**<
                                     * The subscription request has been denied or a
                                     * previously-granted subscription has been cancelled.
                                     */
    STANZA_MESSAGE_CHAT,            /**<
                                     * The message is sent in the context of a one-to-one chat
                                     * conversation.  A compliant client SHOULD present the message in an
                                     * interface enabling one-to-one chat between the two parties,
                                     * including an appropriate conversation history.
                                     */
    STANZA_MESSAGE_ERROR,           /**<
                                     * An error has occurred related to a previous message sent
                                     * by the sender (for details regarding stanza error syntax, refer to
                                     * [XMPP-CORE]).  A compliant client SHOULD present an appropriate
                                     * interface informing the sender of the nature of the error.
                                     */
    STANZA_MESSAGE_GROUPCHAT,       /**<
                                     * The message is sent in the context of a multi-user
                                     * chat environment (similar to that of [IRC]).  A compliant client
                                     * SHOULD present the message in an interface enabling many-to-many
                                     * chat between the parties, including a roster of parties in the
                                     * chatroom and an appropriate conversation history.  Full definition
                                     * of XMPP-based groupchat protocols is out of scope for this memo.
                                     */
    STANZA_MESSAGE_HEADLINE,        /**<
                                     * The message is probably generated by an automated
                                     * service that delivers or broadcasts content (news, sports, market
                                     * information, RSS feeds, etc.).  No reply to the message is
                                     * expected, and a compliant client SHOULD present the message in an
                                     * interface that appropriately differentiates the message from
                                     * standalone messages, chat sessions, or groupchat sessions (e.g.,
                                     * by not providing the recipient with the ability to reply).
                                     */
    STANZA_MESSAGE_NORMAL           /**<
                                     * The message is a single message that is sent outside the
                                     * context of a one-to-one conversation or groupchat, and to which it
                                     * is expected that the recipient will reply.  A compliant client
                                     * SHOULD present the message in an interface enabling the recipient
                                     * to reply, but without a conversation history.
                                     */
  };

  /**
   * Describes types of stanza errors.
   */
  enum StanzaErrorType
  {
    ST_TYPE_CANCEL,                   /**< Do not retry (the error is unrecoverable). */
    ST_TYPE_CONTINUE,                 /**< Proceed (the condition was only a warning). */
    ST_TYPE_MODIFY,                   /**< Retry after changing the data sent. */
    ST_TYPE_AUTH,                     /**< Retry after providing credentials. */
    ST_TYPE_WAIT,                     /**< Retry after waiting (the error is temporary). */
  };

  /**
   * Describes the defined stanza error conditions of RFC 3920.
   */
  enum StanzaError
  {
    ST_ERROR_BAD_REQUEST,           /**< The sender has sent XML that is malformed or that cannot be
                                     * processed (e.g., an IQ stanza that includes an unrecognized value
                                     * of the 'type' attribute); the associated error type SHOULD be
                                     * "modify". */
    ST_ERROR_CONFLICT,              /**< Access cannot be granted because an existing resource or session
                                     * exists with the same name or address; the associated error type
                                     * SHOULD be "cancel". */
    ST_ERROR_FEATURE_NOT_IMPLEMENTED,/**< The feature requested is not implemented by the recipient or server
                                     * and therefore cannot be processed; the associated error type SHOULD be
                                     * "cancel". */
    ST_ERROR_FORBIDDEN,             /**< The requesting entity does not possess the required permissions to
                                     * perform the action; the associated error type SHOULD be "auth". */
    ST_ERROR_GONE,                  /**< The recipient or server can no longer be contacted at this address
                                     * (the error stanza MAY contain a new address in the XML character data
                                     * of the &lt;gone/&gt; element); the associated error type SHOULD be
                                     * "modify". */
    ST_ERROR_INTERNAL_SERVER_ERROR, /**< The server could not process the stanza because of a
                                     * misconfiguration or an otherwise-undefined internal server error; the
                                     * associated error type SHOULD be "wait". */
    ST_ERROR_ITEM_NOT_FOUND,        /**< The addressed JID or item requested cannot be found; the associated
                                     * error type SHOULD be "cancel". */
    ST_ERROR_JID_MALFORMED,         /**< The sending entity has provided or communicated an XMPP address
                                     * (e.g., a value of the 'to' attribute) or aspect thereof (e.g., a
                                     * resource identifier) that does not adhere to the syntax defined in
                                     * Addressing Scheme (Section 3); the associated error type SHOULD be
                                     * "modify". */
    ST_ERROR_NOT_ACCEPTABLE,        /**< The recipient or server understands the request but is refusing to
                                     * process it because it does not meet criteria defined by the recipient
                                     * or server (e.g., a local policy regarding acceptable words in
                                     * messages); the associated error type SHOULD be "modify". */
    ST_ERROR_NOT_ALLOWED,           /**< The recipient or server does not allow any entity to perform the
                                     * action; the associated error type SHOULD be "cancel". */
    ST_ERROR_NOT_AUTHORIZED,        /**< The sender must provide proper credentials before being allowed to
                                     * perform the action, or has provided improper credentials; the
                                     * associated error type SHOULD be "auth". */
    ST_ERROR_PAYMENT_REQUIRED,      /**< The requesting entity is not authorized to access the requested
                                     * service because payment is required; the associated error type SHOULD
                                     * be "auth". */
    ST_ERROR_RECIPIENT_UNAVAILABLE, /**< The intended recipient is temporarily unavailable; the associated
                                     * error type SHOULD be "wait" (note: an application MUST NOT return this
                                     * error if doing so would provide information about the intended
                                     * recipient's network availability to an entity that is not authorized
                                     * to know such information). */
    ST_ERROR_REDIRECT,              /**< The recipient or server is redirecting requests for this information
                                     * to another entity, usually temporarily (the error stanza SHOULD
                                     * contain the alternate address, which MUST be a valid JID, in the XML
                                     * character data of the <redirect/> element); the associated error type
                                     * SHOULD be "modify". */
    ST_ERROR_REGISTRATION_REQUIRED, /**< The requesting entity is not authorized to access the requested
                                     * service because registration is required; the associated error type
                                     * SHOULD be "auth". */
    ST_ERROR_REMOTE_SERVER_NOT_FOUND,/**< A remote server or service specified as part or all of the JID of
                                     * the intended recipient does not exist; the associated error type
                                     * SHOULD be "cancel". */
    ST_ERROR_REMOTE_SERVER_TIMEOUT, /**< A remote server or service specified as part or all of the JID of
                                     * the intended recipient (or required to fulfill a request) could not be
                                     * contacted within a reasonable amount of time; the associated error
                                     * type SHOULD be "wait". */
    ST_ERROR_RESOURCE_CONSTRAINT,   /**< The server or recipient lacks the system resources necessary to
                                     * service the request; the associated error type SHOULD be "wait". */
    ST_ERROR_SERVICE_UNAVAILABLE,   /**< The server or recipient does not currently provide the requested
                                     * service; the associated error type SHOULD be "cancel". */
    ST_ERROR_SUBSCRIBTION_REQUIRED, /**< The requesting entity is not authorized to access the requested
                                     * service because a subscription is required; the associated error type
                                     * SHOULD be "auth". */
    ST_ERROR_UNDEFINED_CONDITION,   /**< The error condition is not one of those defined by the other
                                     * conditions in this list; any error type may be associated with this
                                     * condition, and it SHOULD be used only in conjunction with an
                                     * application-specific condition. */
    ST_ERROR_UNEXPECTED_REQUEST,    /**< The recipient or server understood the request but was not expecting
                                     * it at this time (e.g., the request was out of order); the associated
                                     * error type SHOULD be "wait". */
  };

  /**
   * Describes the possible 'available presence' types.
   */
  enum PresenceStatus
  {
    PRESENCE_UNKNOWN,               /**< Unknown status. */
    PRESENCE_AVAILABLE,             /**< The entity or resource is online and available. */
    PRESENCE_CHAT,                  /**< The entity or resource is actively interested in chatting. */
    PRESENCE_AWAY,                  /**< The entity or resource is temporarily away. */
    PRESENCE_DND,                   /**< The entity or resource is busy (dnd = "Do Not Disturb"). */
    PRESENCE_XA,                    /**< The entity or resource is away for an extended period (xa =
                                     * "eXtended Away"). */
    PRESENCE_UNAVAILABLE,           /**< The entity or resource if offline. */
  };

  /**
   * Describes the verification results of a certificate.
   */
  enum CertStatus
  {
    CERT_OK              =  0,      /**< The certificate is valid and trusted. */
    CERT_INVALID         =  1,      /**< The certificate is not trusted. */
    CERT_SIGNER_UNKNOWN  =  2,      /**< The certificate hasn't got a known issuer. */
    CERT_REVOKED         =  4,      /**< The certificate has been revoked. */
    CERT_EXPIRED         =  8,      /**< The certificate has expired. */
    CERT_NOT_ACTIVE      = 16,      /**< The certifiacte is not yet active. */
    CERT_WRONG_PEER      = 32,      /**< The certificate has not been issued for the
                                     * peer we're connected to. */
    CERT_SIGNER_NOT_CA   = 64,      /**< The signer is not a CA. */
  };

  /**
   * Describes the certificate presented by the peer.
   */
  struct CertInfo
  {
    int status;                     /**< Bitwise or'ed CertStatus or CERT_OK. */
    bool chain;                     /**< Determines whether the cert chain verified ok. */
    std::string issuer;             /**< The name of the issuing entity.*/
    std::string server;             /**< The server the certificate has been issued for. */
    int date_from;                  /**< The date from which onwards the certificate is valid. */
    int date_to;                    /**< The date up to which the certificate is valid. */
    std::string protocol;           /**< The encryption protocol used for the connection. */
    std::string cipher;             /**< The cipher used for the connection. */
    std::string mac;                /**< The MAC used for the connection. */
    std::string compression;        /**< The compression used for the connection. */
  };

  /**
   * Describes the defined SASL error conditions.
   */
  enum AuthenticationError
  {
    SASL_ABORTED,                   /**< The receiving entity acknowledges an &lt;abort/&gt; element sent
                                     * by the initiating entity; sent in reply to the &lt;abort/&gt;
                                     * element. */
    SASL_INCORRECT_ENCODING,        /**< The data provided by the initiating entity could not be processed
                                     * because the [BASE64] encoding is incorrect (e.g., because the encoding
                                     * does not adhere to the definition in Section 3 of [BASE64]); sent in
                                     * reply to a &lt;response/&gt; element or an &lt;auth/&gt; element with
                                     * initial response data. */
    SASL_INVALID_AUTHZID,           /**< The authzid provided by the initiating entity is invalid, either
                                     * because it is incorrectly formatted or because the initiating entity
                                     * does not have permissions to authorize that ID; sent in reply to a
                                     * &lt;response/&gt; element or an &lt;auth/&gt; element with initial
                                     * response data.*/
    SASL_INVALID_MECHANISM,         /**< The initiating entity did not provide a mechanism or requested a
                                     * mechanism that is not supported by the receiving entity; sent in reply
                                     * to an &lt;auth/&gt; element. */
    SASL_MECHANISM_TOO_WEAK,        /**< The mechanism requested by the initiating entity is weaker than
                                     * server policy permits for that initiating entity; sent in reply to a
                                     * &lt;response/&gt; element or an &lt;auth/&gt; element with initial
                                     * response data. */
    SASL_NOT_AUTHORIZED,            /**< The authentication failed because the initiating entity did not
                                     * provide valid credentials (this includes but is not limited to the
                                     * case of an unknown username); sent in reply to a &lt;response/&gt;
                                     * element or an &lt;auth/&gt; element with initial response data. */
    SASL_TEMPORARY_AUTH_FAILURE,    /**< The authentication failed because of a temporary error condition
                                     * within the receiving entity; sent in reply to an &lt;auth/&gt; element
                                     * or &lt;response/&gt; element. */
    NONSASL_CONFLICT,               /**< JEP-0078: Resource Conflict */
    NONSASL_NOT_ACCEPTABLE,         /**< JEP-0078: Required Information Not Provided */
    NONSASL_NOT_AUTHORIZED,         /**< JEP-0078: Incorrect Credentials */
  };

  /**
   * A list of strings.
   */
  typedef std::list<std::string> StringList;

  /**
   * A map of strings.
   */
  typedef std::map<std::string, std::string> StringMap;

};

#endif // GLOOX_H__
