
#define ENTITY_TYPES(TYPE)                                                     \
  TYPE(ENTITY_TYPE_COIN, coin)                                                 \
  TYPE(ENTITY_TYPE_PLAYER, player)

// All entity types share the same struct. Calling ENTITY_DEFINE() defines that
// struct with the fields required by high_impact and the additional fields
// specified here.

ENTITY_DEFINE();

// The entity_message_t is used with the entity_message() function. You can
// extend this as you wish.

typedef enum {
  EM_INVALID,
} entity_message_t;
